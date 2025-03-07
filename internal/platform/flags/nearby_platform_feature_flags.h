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

#ifndef THIRD_PARTY_NEARBY_INTERNAL_PLATFORM_FLAGS_NEARBY_PLATFORM_FEATURE_FLAGS_H_
#define THIRD_PARTY_NEARBY_INTERNAL_PLATFORM_FLAGS_NEARBY_PLATFORM_FEATURE_FLAGS_H_

#include "absl/strings/string_view.h"
#include "internal/flags/flag.h"

namespace nearby {
namespace platform {
namespace config_package_nearby {

constexpr absl::string_view kConfigPackage = "nearby";

// The Nearby Platform features.
namespace nearby_platform_feature {

// Disable/Enable win32 socket implementation for Wi-Fi hotspot.
constexpr auto kEnableHotspotWin32Socket =
    flags::Flag<bool>(kConfigPackage, "45401992", true);

}  // namespace nearby_platform_feature
}  // namespace config_package_nearby
}  // namespace platform
}  // namespace nearby

#endif  // THIRD_PARTY_NEARBY_INTERNAL_PLATFORM_FLAGS_NEARBY_PLATFORM_FEATURE_FLAGS_H_
