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
#include <string>

#include "gtest/gtest.h"
#include "fastpair/ui/fast_pair/fake_fast_pair_presenter.h"
#include "fastpair/ui/fast_pair/fast_pair_notification_controller.h"
#include "fastpair/ui/ui_broker.h"

namespace nearby {
namespace fastpair {
namespace {

constexpr absl::string_view kModelId = "718C17";
constexpr absl::string_view kAddress = "74:74:46:01:6C:21";

class UIBrokerImplTest : public ::testing::Test, public UIBroker::Observer {
 protected:
  UIBrokerImplTest() {
    presenter_factory_ = std::make_unique<FakeFastPairPresenterFactory>();
    FastPairPresenterImpl::Factory::SetFactoryForTesting(
        presenter_factory_.get());
    ui_broker_ = std::make_unique<UIBrokerImpl>();
    ui_broker_->AddObserver(this);
  }

  void TriggerOnDiscoveryAction(FastPairDevice& device) {
    ui_broker_->NotifyDiscoveryAction(device);
  }

  void OnDiscoveryAction(const FastPairDevice& device) override {
    discovery_action_ = true;
  }

  std::unique_ptr<UIBroker> ui_broker_;
  FastPairNotificationController notification_controller_;
  std::unique_ptr<FakeFastPairPresenterFactory> presenter_factory_;
  bool discovery_action_ = false;
};

TEST_F(UIBrokerImplTest, ShowDiscovery) {
  FastPairDevice device(kModelId, kAddress, Protocol::kFastPairInitialPairing);
  ui_broker_->ShowDiscovery(device, notification_controller_);
  EXPECT_TRUE(
      presenter_factory_->fake_fast_pair_presenter()->show_deiscovery());
}

TEST_F(UIBrokerImplTest, NotifyDiscoveryAction) {
  FastPairDevice device(kModelId, kAddress, Protocol::kFastPairInitialPairing);
  TriggerOnDiscoveryAction(device);
  EXPECT_TRUE(discovery_action_);
}

TEST_F(UIBrokerImplTest, NotifyDiscoveryAction_WithEmptyObsesrvers) {
  FastPairDevice device(kModelId, kAddress, Protocol::kFastPairInitialPairing);
  ui_broker_->RemoveObserver(this);
  TriggerOnDiscoveryAction(device);
  EXPECT_FALSE(discovery_action_);
}

}  // namespace
}  // namespace fastpair
}  // namespace nearby
