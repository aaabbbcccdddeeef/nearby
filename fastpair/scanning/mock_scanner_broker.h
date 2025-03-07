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
#ifndef THIRD_PARTY_NEARBY_FASTPAIR_SCANNING_MOCK_SCANNER_BROKER_H_
#define THIRD_PARTY_NEARBY_FASTPAIR_SCANNING_MOCK_SCANNER_BROKER_H_

#include "gmock/gmock.h"
#include "fastpair/common/fast_pair_device.h"
#include "fastpair/scanning/scanner_broker.h"
#include "internal/base/observer_list.h"

namespace nearby {
namespace fastpair {

class MockScannerBroker : public ScannerBroker {
 public:
  MOCK_METHOD(void, StartScanning, (Protocol), (override));
  MOCK_METHOD(void, StopScanning, (Protocol), (override));

  void AddObserver(Observer* observer) override {
    observers_.AddObserver(observer);
  }

  void RemoveObserver(Observer* observer) override {
    observers_.RemoveObserver(observer);
  }

  void NotifyDeviceFound(const FastPairDevice& device) {
    for (auto& observer : observers_.GetObservers()) {
      observer->OnDeviceFound(device);
    }
  }

  void NotifyDeviceLost(const FastPairDevice& device) {
    for (auto& observer : observers_.GetObservers()) {
      observer->OnDeviceLost(device);
    }
  }

 private:
  ObserverList<Observer> observers_;
};

}  // namespace fastpair
}  // namespace nearby

#endif  // THIRD_PARTY_NEARBY_FASTPAIR_SCANNING_MOCK_SCANNER_BROKER_H_
