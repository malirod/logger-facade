#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "logger.h"

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::InitGoogleMock(&argc, argv);
  INIT_LOGGER("logger.cfg");
  return RUN_ALL_TESTS();
}
