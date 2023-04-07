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

#import "internal/platform/implementation/apple/preferences_repository.h"

#include "absl/synchronization/mutex.h"
#include "third_party/json/include/nlohmann/json.hpp"

namespace nearby::apple {

nlohmann::json PreferencesRepository::LoadPreferences() {
  absl::MutexLock lock(&mutex_);
  return nlohmann::json::object();
}

bool PreferencesRepository::SavePreferences(nlohmann::json preferences) {
  absl::MutexLock lock(&mutex_);
  return false;
}

}  // namespace nearby::apple