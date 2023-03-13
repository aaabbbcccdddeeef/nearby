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

#ifndef THIRD_PARTY_NEARBY_INTERNAL_WEAVE_PACKET_COUNTER_GENERATOR_H_
#define THIRD_PARTY_NEARBY_INTERNAL_WEAVE_PACKET_COUNTER_GENERATOR_H_

#include "absl/base/thread_annotations.h"
#include "internal/platform/mutex.h"
#include "internal/platform/mutex_lock.h"
#include "internal/weave/packet.h"

namespace nearby {
namespace weave {

class PacketCounterGenerator {
 public:
  int Next() {
    MutexLock lock(&mutex_);
    int nextPacketCounter = packet_counter_;
    UpdateLocked(packet_counter_);
    return nextPacketCounter;
  }

  void Update(int last_packet_counter) {
    MutexLock lock(&mutex_);
    UpdateLocked(last_packet_counter);
  }

  void Reset() {
    MutexLock lock(&mutex_);
    packet_counter_ = 0;
  }

 private:
  void UpdateLocked(int last_packet_counter)
      ABSL_EXCLUSIVE_LOCKS_REQUIRED(mutex_) {
    if (last_packet_counter == Packet::kMaxPacketCounter) {
      packet_counter_ = 0;
    } else {
      ++packet_counter_;
    }
  }

  Mutex mutex_;
  int packet_counter_ ABSL_GUARDED_BY(mutex_) = 0;
};

}  // namespace weave
}  // namespace nearby

#endif  // THIRD_PARTY_NEARBY_INTERNAL_WEAVE_PACKET_COUNTER_GENERATOR_H_
