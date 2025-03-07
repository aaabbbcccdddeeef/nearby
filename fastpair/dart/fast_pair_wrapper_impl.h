// Copyright 2022 Google LLC
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

#ifndef THIRD_PARTY_NEARBY_FASTPAIR_DART_FAST_PAIR_WRAPPER_IMPL_H_
#define THIRD_PARTY_NEARBY_FASTPAIR_DART_FAST_PAIR_WRAPPER_IMPL_H_

#include <functional>
#include <memory>

#include "fastpair/dart/fast_pair_wrapper.h"
#include "fastpair/scanning/scanner_broker.h"

namespace nearby {
namespace fastpair {

class FastPairScannerImpl;

class FastPairWrapperImpl : public FastPairWrapper {
 public:
  explicit FastPairWrapperImpl();
  ~FastPairWrapperImpl() override;

  bool IsScanning() override;
  bool IsPairing() override;
  bool IsServerAccessing() override;
  void StartScan() override;

 private:
  std::unique_ptr<ScannerBroker> scanner_broker_;

  // True if we are currently scanning for remote devices.
  bool is_scanning_ = false;
  bool is_connecting_ = false;
  bool is_server_access_ = false;
};

}  // namespace fastpair
}  // namespace nearby

#endif  // THIRD_PARTY_NEARBY_FASTPAIR_DART_FAST_PAIR_WRAPPER_IMPL_H_
