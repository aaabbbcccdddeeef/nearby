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

#ifndef THIRD_PARTY_NEARBY_INTERNAL_WEAVE_SINGLE_PACKET_WRITE_REQUEST_H_
#define THIRD_PARTY_NEARBY_INTERNAL_WEAVE_SINGLE_PACKET_WRITE_REQUEST_H_

#include <utility>

#include "absl/status/status.h"
#include "internal/platform/future.h"
#include "internal/weave/packet.h"
#include "internal/weave/write_request.h"

namespace nearby {
namespace weave {

class SinglePacketWriteRequest : public WriteRequest {
 public:
  explicit SinglePacketWriteRequest(Packet packet)
      : packet_(std::move(packet)) {}
  bool IsStarted() const override { return is_started_; }
  bool IsFinished() const override { return is_started_; }
  absl::StatusOr<Packet> NextPacket(int max_packet_size) override {
    if (IsFinished()) {
      return absl::ResourceExhaustedError("Packet was already sent");
    }
    is_started_ = true;
    return std::move(packet_);
  }

  void Reset() override { is_started_ = false; }
  void SetFuture(absl::Status status) override { result_.Set(status); }
  nearby::Future<absl::Status> GetFutureResult() override { return result_; }

 private:
  nearby::Future<absl::Status> result_;
  Packet packet_;
  bool is_started_ = false;
};

}  // namespace weave
}  // namespace nearby

#endif  // THIRD_PARTY_NEARBY_INTERNAL_WEAVE_SINGLE_PACKET_WRITE_REQUEST_H_
