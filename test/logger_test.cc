#include "logger.h"
#include "gtest/gtest.h"

TEST(TestLogger, DummyTest) {
  blsb::LoggerType local_logger;
  LOG_DEBUG(local_logger) << "lamer";
  EXPECT_TRUE(true == true);
}
