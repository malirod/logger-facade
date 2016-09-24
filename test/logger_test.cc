#include "gtest/gtest.h"
#include "logger.h"

TEST(TestLogger, DummyTest) {

  BOOST_LOG_TRIVIAL(trace) << "A trace severity message";
  EXPECT_TRUE(true == true);
}
