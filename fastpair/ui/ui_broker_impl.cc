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

#include "fastpair/ui/ui_broker_impl.h"

#include <memory>

#include "fastpair/common/fast_pair_device.h"
#include "fastpair/common/protocol.h"
#include "fastpair/ui/fast_pair/fast_pair_presenter.h"
#include "fastpair/ui/fast_pair/fast_pair_presenter_impl.h"
#include "internal/platform/logging.h"

namespace nearby {
namespace fastpair {

UIBrokerImpl::UIBrokerImpl()
    : fast_pair_presenter_(FastPairPresenterImpl::Factory::Create()) {}

void UIBrokerImpl::AddObserver(Observer* observer) {
  observers_.AddObserver(observer);
}

void UIBrokerImpl::RemoveObserver(Observer* observer) {
  observers_.RemoveObserver(observer);
}

void UIBrokerImpl::ShowDiscovery(
    const FastPairDevice& device,
    FastPairNotificationController& notification_controller) {
  switch (device.GetProtocol()) {
    case Protocol::kFastPairInitialPairing:
    case Protocol::kFastPairSubsequentPairing:
      fast_pair_presenter_->ShowDiscovery(device, notification_controller);
      break;
    case Protocol::kFastPairRetroactivePairing:
      NEARBY_LOGS(ERROR) << __func__
                         << ": Retroactive Pairing should not show Halfsheet.";
      break;
  }
}

void UIBrokerImpl::NotifyDiscoveryAction(const FastPairDevice& device) {
  for (auto& observer : observers_.GetObservers())
    observer->OnDiscoveryAction(device);
}

}  // namespace fastpair
}  // namespace nearby
