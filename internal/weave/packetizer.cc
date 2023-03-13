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

#include "internal/weave/packetizer.h"

#include <utility>

#include "absl/status/status.h"
#include "internal/platform/byte_array.h"
#include "internal/platform/mutex_lock.h"
#include "internal/weave/packet.h"

namespace nearby {
namespace weave {

/*
Joins the Packets into a message, returning the message once the last packet is
passed to this function.

This function returns absl::UnavailableError if the packet provided
is not the first or last packet, as the full message is not available just yet.

This function returns absl::InvalidArgumentError if the packet joined is not
the first packet (if no prior packets have been joined) or if the packet being
joined is the first packet, but there is already a message in progress of
reconstruction.
*/
absl::StatusOr<ByteArray> Packetizer::Join(const Packet&& packet) {
  MutexLock lock(&mutex_);
  if (pending_payloads_.empty() && !packet.IsFirstPacket()) {
    return absl::InvalidArgumentError(
        "First packet must be the first packet added.");
  }
  if (!pending_payloads_.empty() && packet.IsFirstPacket()) {
    return absl::InvalidArgumentError(
        "First packet cannot be added if there are pending packets.");
  }
  pending_payloads_.append(packet.GetPayload());
  if (!packet.IsLastPacket()) {
    return absl::UnavailableError("No last packet found yet.");
  }
  ByteArray message = ByteArray(std::move(pending_payloads_));
  pending_payloads_ = "";
  return message;
}

// Resets the packetizer, clearing any pending messages.
void Packetizer::Reset() {
  MutexLock lock(&mutex_);
  pending_payloads_ = "";
}

}  // namespace weave
}  // namespace nearby
