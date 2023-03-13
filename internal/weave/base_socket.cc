// Copyright 2023 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "internal/weave/base_socket.h"

#include <deque>
// NOLINTNEXTLINE
#include <future>
#include <string>
#include <utility>

#include "absl/base/thread_annotations.h"
#include "absl/status/status.h"
#include "absl/strings/str_format.h"
#include "internal/platform/byte_array.h"
#include "internal/platform/logging.h"
#include "internal/platform/mutex_lock.h"
#include "internal/weave/connection.h"
#include "internal/weave/message_write_request.h"
#include "internal/weave/packet.h"
#include "internal/weave/single_packet_write_request.h"
#include "internal/weave/write_request.h"

namespace nearby {
namespace weave {

// WriteRequestExecutor
void BaseSocket::WriteRequestExecutor::TryWrite() {
  if (current_ != nullptr) {
    return;
  }
  NEARBY_LOGS(INFO) << "Current is null";
  WriteRequest* request = nullptr;
  {
    MutexLock lock(&mutex_);
    if (!controls_.empty()) {
      request = &(*controls_.begin());
    }
  }

  if (request == nullptr && socket_->IsConnected()) {
    NEARBY_LOGS(INFO) << "picking message";
    {
      MutexLock lock(&mutex_);
      if (!messages_.empty()) {
        request = &(*messages_.begin());
      }
    }
  } else {
    ResetFirstMessageIfStarted();
  }
  NEARBY_LOGS(INFO) << "Set request";
  if (request != nullptr) {
    if (request->IsFinished()) {
      NEARBY_LOGS(INFO) << "request finished";
      return;
    }
    current_ = request;
  }
  if (current_ != nullptr) {
    absl::StatusOr<Packet> packet =
        current_->NextPacket(socket_->max_packet_size_);
    if (packet.ok()) {
      if (!packet.value()
               .SetPacketCounter(socket_->packet_counter_generator_.Next())
               .ok()) {
        return;
      }
      NEARBY_LOGS(INFO) << "transmitting pkt";
      socket_->connection_->Transmit(packet.value().GetBytes());
    } else {
      NEARBY_LOGS(WARNING) << packet.status();
      return;
    }
  }
}

void BaseSocket::WriteRequestExecutor::OnWriteResult(absl::Status status) {
  NEARBY_LOGS(INFO) << "OnWriteResult";
  socket_->RunOnSocketThread(
      "OnWriteResult", [this, status]() ABSL_EXCLUSIVE_LOCKS_REQUIRED(
                           executor_) ABSL_LOCKS_EXCLUDED(mutex_) mutable {
        {
          MutexLock lock(&mutex_);
          if (current_ == nullptr) {
            return;
          }
          if (current_->IsFinished()) {
            NEARBY_LOGS(INFO) << "OnWriteResult current finished";
            current_->SetFuture(status);
            if (!messages_.empty() && current_ == &(*messages_.begin())) {
              NEARBY_LOGS(INFO) << "remove message";
              messages_.pop_front();
            } else if (!controls_.empty() &&
                       current_ == &(*controls_.begin())) {
              NEARBY_LOGS(INFO) << "remove control";
              controls_.pop_front();
            }
          }
          NEARBY_LOGS(INFO) << "controls size: " << controls_.size();
          NEARBY_LOGS(INFO) << "messages size: " << messages_.size();
          current_ = nullptr;
        }
        TryWrite();
      });
}

std::future<absl::Status> BaseSocket::WriteRequestExecutor::Queue(
    ByteArray message) {
  MessageWriteRequest request = MessageWriteRequest(message.AsStringView());
  std::future<absl::Status> ret = request.GetFutureResult();

  socket_->RunOnSocketThread(
      "TryWriteMessage", [this, request = std::move(request)]()
                             ABSL_EXCLUSIVE_LOCKS_REQUIRED(executor_) mutable {
                               messages_.push_back(std::move(request));
                               TryWrite();
                             });
  return ret;
}

std::future<absl::Status> BaseSocket::WriteRequestExecutor::Queue(
    Packet packet) {
  SinglePacketWriteRequest request =
      SinglePacketWriteRequest(std::move(packet));
  std::future<absl::Status> ret = request.GetFutureResult();

  socket_->RunOnSocketThread(
      "TryWriteControl", [this, request = std::move(request)]()
                             ABSL_EXCLUSIVE_LOCKS_REQUIRED(executor_) mutable {
                               controls_.push_back(std::move(request));
                               TryWrite();
                             });
  return ret;
}

void BaseSocket::WriteRequestExecutor::ResetFirstMessageIfStarted() {
  auto latest = messages_.begin();
  if (latest != messages_.end() && latest->IsStarted()) {
    latest->Reset();
  }
  NEARBY_LOGS(INFO) << "reset first message";
}

// BaseSocket implementation
bool BaseSocket::IsRemotePacketCounterExpected(int counter) {
  int expectedPacketCounter = remote_packet_counter_generator_.Next();
  if (counter == expectedPacketCounter) {
    return true;
  } else {
    socket_callback_.on_error_cb(absl::DataLossError(absl::StrFormat(
        "expected remote packet counter %d for packet but got %d",
        expectedPacketCounter, counter)));
    return false;
  }
}

BaseSocket::BaseSocket(Connection* connection, SocketCallback&& callback)
    : write_request_executor_(WriteRequestExecutor(this)) {
  connection_ = connection;
  socket_callback_ = std::move(callback);
  connection_->Initialize(
      {.on_transmit_cb =
           [this](absl::Status status) mutable {
             write_request_executor_.OnWriteResult(status);
             if (!status.ok()) {
               DisconnectInternal(status);
             }
           },
       .on_remote_transmit_cb =
           [this](std::string message) mutable {
             if (message.empty()) {
               DisconnectInternal(absl::InvalidArgumentError("Empty packet!"));
               return;
             }
             absl::StatusOr<Packet> packet{
                 Packet::FromBytes(ByteArray(message))};
             if (!packet.ok()) {
               DisconnectInternal(packet.status());
               return;
             }
             bool isRemotePacketCounterExpected =
                 IsRemotePacketCounterExpected(packet->GetPacketCounter());
             if (packet->IsControlPacket()) {
               if (!isRemotePacketCounterExpected) {
                 remote_packet_counter_generator_.Update(
                     packet->GetPacketCounter());
               }
               OnReceiveControlPacket(std::move(*packet));
             } else {
               if (isRemotePacketCounterExpected) {
                 OnReceiveDataPacket(std::move(*packet));
               } else {
                 Disconnect();
               }
             }
           },
       .on_disconnected_cb = [this]() mutable { DisconnectQuietly(); }});
}

BaseSocket::~BaseSocket() {
  NEARBY_LOGS(INFO) << "~BaseSocket";
  ShutDown();
}

void BaseSocket::ShutDown() {
  executor_.Shutdown();
  NEARBY_LOGS(INFO) << "BaseSocket gone.";
}

void BaseSocket::Disconnect() {
  if (!is_disconnecting_) {
    is_disconnecting_ = true;
    RunOnSocketThread("SendErrorPacket", [this]() {
      write_request_executor_.Queue(Packet::CreateErrorPacket());
      NEARBY_LOGS(INFO) << "Queued error packet";
      DisconnectQuietly();
    });
  }
}

void BaseSocket::DisconnectQuietly() {
  bool wasConnectingOrConnected = IsConnectingOrConnected();
  connected_ = false;
  if (wasConnectingOrConnected) {
    socket_callback_.on_disconnected_cb();
  }
  NEARBY_LOGS(INFO) << "Called socket disconn";
  write_request_executor_.ResetFirstMessageIfStartedExternal();
  RunOnSocketThread("ResetDisconnectQuietly",
                    [this]() ABSL_EXCLUSIVE_LOCKS_REQUIRED(executor_) mutable {
                      packetizer_.Reset();
                      packet_counter_generator_.Reset();
                      remote_packet_counter_generator_.Reset();
                    });
  NEARBY_LOGS(INFO) << "scheduled reset";
}

void BaseSocket::OnReceiveDataPacket(Packet packet) {
  absl::StatusOr<ByteArray> message = packetizer_.Join(std::move(packet));
  if (message.ok()) {
    socket_callback_.on_receive_cb(message.value().string_data());
  } else if (message.status().code() == absl::StatusCode::kInvalidArgument) {
    DisconnectInternal(message.status());
  }
}

std::future<absl::Status> BaseSocket::Write(ByteArray message) {
  return write_request_executor_.Queue(message);
}

std::future<absl::Status> BaseSocket::WriteControlPacket(Packet packet) {
  return write_request_executor_.Queue(std::move(packet));
}

void BaseSocket::DisconnectInternal(absl::Status status) {
  socket_callback_.on_error_cb(status);
  Disconnect();
}

bool BaseSocket::IsConnected() { return connected_; }

bool BaseSocket::IsConnectingOrConnected() { return connected_; }

void BaseSocket::OnConnected(int new_max_packet_size) {
  max_packet_size_ = new_max_packet_size;
  bool was_connected = IsConnected();
  connected_ = true;
  is_disconnecting_ = false;
  if (!was_connected) {
    socket_callback_.on_connected_cb();
  }
  write_request_executor_.TryWriteExternal();
}

}  // namespace weave
}  // namespace nearby
