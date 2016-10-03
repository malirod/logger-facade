#include "logger.h"
#include "gtest/gtest.h"

TEST(TestLogger, DummyTest) {
  LOG_DEBUG << "lamer";
  EXPECT_TRUE(true == true);
}
