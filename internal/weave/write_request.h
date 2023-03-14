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

#ifndef THIRD_PARTY_NEARBY_INTERNAL_WEAVE_WRITE_REQUEST_H_
#define THIRD_PARTY_NEARBY_INTERNAL_WEAVE_WRITE_REQUEST_H_

// NOLINTNEXTLINE
#include <future>
#include <memory>

#include "internal/weave/packet.h"

namespace nearby {
namespace weave {

class WriteRequest {
 public:
  virtual ~WriteRequest() = default;
  virtual bool IsStarted() const = 0;
  virtual bool IsFinished() const = 0;
  virtual absl::StatusOr<Packet> NextPacket(int max_packet_size) = 0;
  virtual void Reset() = 0;
  virtual std::future<absl::Status> GetFutureResult() = 0;
  virtual void SetFuture(absl::Status status) = 0;
};

}  // namespace weave
}  // namespace nearby

#endif  // THIRD_PARTY_NEARBY_INTERNAL_WEAVE_WRITE_REQUEST_H_
