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

#include "fastpair/common/context_impl.h"

#include <memory>
#include <utility>

#include "internal/platform/clock_impl.h"
#include "internal/platform/device_info_impl.h"
#include "internal/platform/timer_impl.h"
#include "internal/platform/task_runner_impl.h"

namespace nearby {
namespace fastpair {

ContextImpl::ContextImpl()
    : clock_(std::make_unique<ClockImpl>()),
      device_info_(std::make_unique<DeviceInfoImpl>()) {}

Clock* ContextImpl::GetClock() const { return clock_.get(); }

std::unique_ptr<Timer> ContextImpl::CreateTimer() {
  return std::make_unique<TimerImpl>();
}

DeviceInfo* ContextImpl::GetDeviceInfo() const { return device_info_.get(); }

std::unique_ptr<TaskRunner> ContextImpl::CreateSequencedTaskRunner() const {
  std::unique_ptr<TaskRunner> task_runner = std::make_unique<TaskRunnerImpl>(1);
  return task_runner;
}

std::unique_ptr<TaskRunner> ContextImpl::CreateConcurrentTaskRunner(
    uint32_t concurrent_count) const {
  std::unique_ptr<TaskRunner> task_runner =
      std::make_unique<TaskRunnerImpl>(concurrent_count);
  return task_runner;
}
}  // namespace fastpair
}  // namespace nearby
