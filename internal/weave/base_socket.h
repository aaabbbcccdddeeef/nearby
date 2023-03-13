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

#ifndef THIRD_PARTY_NEARBY_INTERNAL_WEAVE_BASE_SOCKET_H_
#define THIRD_PARTY_NEARBY_INTERNAL_WEAVE_BASE_SOCKET_H_

#include <deque>
// NOLINTNEXTLINE
#include <future>
#include <memory>
#include <string>
#include <utility>

#include "absl/base/thread_annotations.h"
#include "internal/platform/mutex.h"
#include "internal/platform/single_thread_executor.h"
#include "internal/weave/connection.h"
#include "internal/weave/message_write_request.h"
#include "internal/weave/packet.h"
#include "internal/weave/packet_counter_generator.h"
#include "internal/weave/packetizer.h"
#include "internal/weave/single_packet_write_request.h"
#include "internal/weave/socket_callback.h"
#include "internal/weave/write_request.h"

namespace nearby {
namespace weave {

class BaseSocket {
 public:
  BaseSocket(Connection* connection, SocketCallback&& callback);
  virtual ~BaseSocket();
  bool IsConnected();
  void Disconnect();
  std::future<absl::Status> Write(ByteArray message);
  virtual void Connect() = 0;

 protected:
  SocketCallback socket_callback_;
  void OnConnected(int new_max_packet_size);
  void DisconnectInternal(absl::Status status);
  virtual bool IsConnectingOrConnected();
  virtual void DisconnectQuietly();
  virtual void OnReceiveControlPacket(Packet packet) {}
  virtual std::future<absl::Status> WriteControlPacket(Packet packet);
  void OnReceiveDataPacket(Packet packet);
  Connection* connection_;
  void RunOnSocketThread(std::string name, Runnable&& runnable) {
    executor_.Execute(name, std::move(runnable));
  }
  void ShutDown();

 private:
  class WriteRequestExecutor {
   public:
    explicit WriteRequestExecutor(BaseSocket* socket) : socket_(socket) {}
    ~WriteRequestExecutor() { NEARBY_LOGS(INFO) << "~WriteRequestExecutor"; }
    void TryWriteExternal() {
      socket_->RunOnSocketThread(
          "TryWrite",
          [this]() ABSL_EXCLUSIVE_LOCKS_REQUIRED(executor_) { TryWrite(); });
    }
    void OnWriteResult(absl::Status status) ABSL_LOCKS_EXCLUDED(executor_);
    std::future<absl::Status> Queue(ByteArray message)
        ABSL_LOCKS_EXCLUDED(mutex_, executor_);
    std::future<absl::Status> Queue(Packet packet)
        ABSL_LOCKS_EXCLUDED(mutex_, executor_);
    void ResetFirstMessageIfStartedExternal() {
      socket_->RunOnSocketThread("ResetFirstMessageIfStarted",
                                 [this]()
                                     ABSL_EXCLUSIVE_LOCKS_REQUIRED(executor_) {
                                       ResetFirstMessageIfStarted();
                                     });
    }

   private:
    void TryWrite() ABSL_EXCLUSIVE_LOCKS_REQUIRED(executor_)
        ABSL_LOCKS_EXCLUDED(mutex_);
    void ResetFirstMessageIfStarted() ABSL_EXCLUSIVE_LOCKS_REQUIRED(executor_);
    BaseSocket* socket_;
    Mutex mutex_;
    std::deque<SinglePacketWriteRequest> controls_
        ABSL_GUARDED_BY(executor_) = {};
    std::deque<MessageWriteRequest> messages_ ABSL_GUARDED_BY(executor_) = {};
    WriteRequest* current_ = nullptr;
  };
  bool IsRemotePacketCounterExpected(int counter);
  bool connected_ = false;
  bool is_disconnecting_ = false;
  int max_packet_size_;
  Packetizer packetizer_ = {};
  PacketCounterGenerator packet_counter_generator_;
  PacketCounterGenerator remote_packet_counter_generator_;
  WriteRequestExecutor write_request_executor_;
  SingleThreadExecutor executor_;
};

}  // namespace weave
}  // namespace nearby

#endif  // THIRD_PARTY_NEARBY_INTERNAL_WEAVE_BASE_SOCKET_H_
