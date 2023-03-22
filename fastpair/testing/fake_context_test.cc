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

#include "fastpair/testing/fake_context.h"

#include "gmock/gmock.h"
#include "protobuf-matchers/protocol-buffer-matchers.h"
#include "gtest/gtest.h"

namespace nearby {
namespace fastpair {
namespace {

TEST(FakeContext, TestAccessMockContext) {
  FakeContext context;
  EXPECT_NE(context.GetClock(), nullptr);
  EXPECT_NE(context.CreateTimer(), nullptr);
  EXPECT_NE(context.GetDeviceInfo(), nullptr);
  EXPECT_NE(context.CreateSequencedTaskRunner(), nullptr);
  EXPECT_NE(context.CreateConcurrentTaskRunner(5), nullptr);
}

}  // namespace
}  // namespace fastpair
}  // namespace nearby

