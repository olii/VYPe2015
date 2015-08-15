#include "gtest/gtest.h"

using namespace ::testing;

class ExampleTest : public Test {};

TEST_F(ExampleTest,
ShouldBeOK) {
    EXPECT_EQ(1, 1);
}
