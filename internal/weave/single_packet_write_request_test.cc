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

#include "internal/weave/single_packet_write_request.h"

#include <string>
#include <utility>

#include "gmock/gmock.h"
#include "protobuf-matchers/protocol-buffer-matchers.h"
#include "gtest/gtest.h"

namespace nearby {
namespace weave {
namespace {

TEST(SinglePacketWriteRequestTest, WriteRequestIsNotTriviallyConstructible) {
  EXPECT_FALSE(
      std::is_trivially_constructible<SinglePacketWriteRequest>::value);
}

TEST(SinglePacketWriteRequestTest, WriteRequestWorks) {
  Packet packet = Packet::CreateConnectionRequestPacket(1, 1, 15, "").value();
  std::string packet_bytes = packet.GetBytes();
  SinglePacketWriteRequest request =
      SinglePacketWriteRequest(std::move(packet));
  EXPECT_FALSE(request.IsFinished());
  EXPECT_FALSE(request.IsStarted());
  EXPECT_EQ(packet_bytes, request.NextPacket(15).value().GetBytes());
  EXPECT_TRUE(request.IsFinished());
  EXPECT_TRUE(request.IsStarted());
}

TEST(SinglePacketWriteRequestTest, TestReset) {
  Packet packet = Packet::CreateConnectionRequestPacket(1, 1, 15, "").value();
  std::string packet_bytes = packet.GetBytes();
  SinglePacketWriteRequest request =
      SinglePacketWriteRequest(std::move(packet));
  EXPECT_FALSE(request.IsFinished());
  EXPECT_FALSE(request.IsStarted());
  EXPECT_EQ(packet_bytes, request.NextPacket(15).value().GetBytes());
  EXPECT_TRUE(request.IsFinished());
  EXPECT_TRUE(request.IsStarted());
  request.Reset();
  EXPECT_FALSE(request.IsFinished());
  EXPECT_FALSE(request.IsStarted());
}

TEST(SinglePacketWriteRequestTest, TestResourceExhaustion) {
  Packet packet = Packet::CreateConnectionRequestPacket(1, 1, 15, "").value();
  std::string packet_bytes = packet.GetBytes();
  SinglePacketWriteRequest request =
      SinglePacketWriteRequest(std::move(packet));
  EXPECT_FALSE(request.IsFinished());
  EXPECT_FALSE(request.IsStarted());
  EXPECT_EQ(packet_bytes, request.NextPacket(15).value().GetBytes());
  EXPECT_TRUE(request.IsFinished());
  EXPECT_TRUE(request.IsStarted());
  EXPECT_THAT(request.NextPacket(15),
              testing::status::StatusIs(absl::StatusCode::kResourceExhausted));
}

TEST(SinglePacketWriteRequestTest, TestGetSetFuture) {
  Packet packet = Packet::CreateConnectionRequestPacket(1, 1, 15, "").value();
  SinglePacketWriteRequest request =
      SinglePacketWriteRequest(std::move(packet));
  auto result = request.GetFutureResult();
  request.SetFuture(absl::InternalError(""));
  EXPECT_THAT(result.Get().GetResult(),
              testing::status::StatusIs(absl::StatusCode::kInternal));
}

}  // namespace
}  // namespace weave
}  // namespace nearby
