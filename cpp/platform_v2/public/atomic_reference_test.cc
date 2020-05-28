#include "platform_v2/public/atomic_reference.h"

#include "gtest/gtest.h"

namespace location {
namespace nearby {
namespace {

struct BigSizedStruct {
  int data[100]{};
};

enum TestEnum {
  kValue1 = 1,
  kValue2 = 2,
};

enum class ScopedTestEnum {
  kValue1 = 1,
  kValue2 = 2,
};

bool operator==(const BigSizedStruct& a, const BigSizedStruct& b) {
  return memcmp(a.data, b.data, sizeof(BigSizedStruct::data)) == 0;
}

bool operator!=(const BigSizedStruct& a, const BigSizedStruct& b) {
  return !(a == b);
}

}  // namespace

TEST(AtomicReferenceTest, SupportIntegralTypes) {
  AtomicReference<int> atomic_ref({});
  atomic_ref.Set(5);
  EXPECT_EQ(atomic_ref.Get(), 5);
}

TEST(AtomicReferenceTest, SupportEnum) {
  AtomicReference<TestEnum> atomic_ref({});
  atomic_ref.Set(TestEnum::kValue1);
  EXPECT_EQ(atomic_ref.Get(), TestEnum::kValue1);
}

TEST(AtomicReferenceTest, SupportScopedEnum) {
  AtomicReference<ScopedTestEnum> atomic_ref({});
  atomic_ref.Set(ScopedTestEnum::kValue1);
  EXPECT_EQ(atomic_ref.Get(), ScopedTestEnum::kValue1);
}

TEST(AtomicReferenceTest, SetTakesCopyOfValue) {
  // Default constructor is zero-initalizing all data in BigSizedStruct.
  BigSizedStruct v1;
  AtomicReference<BigSizedStruct> atomic_ref({});
  v1.data[0] = 5;  // Changing value before calling set() will affect stored
  v1.data[7] = 3;  // value.
  atomic_ref.Set(v1);
  v1.data[1] = 6;  // Changing value after calling set() will not affect stored
  v1.data[5] = 4;  // value.
  BigSizedStruct v2 = atomic_ref.Get();
  EXPECT_NE(v1, v2);
  v1.data[1] = 0;
  v1.data[5] = 0;
  EXPECT_EQ(v2, v1);
}

TEST(AtomicReferenceTest, SupportObjects) {
  std::string s{"test"};
  AtomicReference<std::string> atomic_ref({});
  atomic_ref.Set(s);
  EXPECT_EQ(s, atomic_ref.Get());
}

}  // namespace nearby
}  // namespace location
