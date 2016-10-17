#include "logger.h"
#include <boost/log/attributes/named_scope.hpp>
#include "gtest/gtest.h"

DECLARE_GLOBAL_GET_LOGGER("LoggerTest.Global")

namespace {

void OutputTestLogLinesAnonymous() {
  LOG_TRACE("Trace global line");
  LOG_DEBUG("Debug global line");
  LOG_INFO("Info global line");
  LOG_WARN("Warn global line");
  LOG_ERROR("Error global line");
  LOG_FATAL("Fatal global line");
}

}  // namespace

namespace Foo {

class Bar {
 public:
  DECLARE_GET_LOGGER("Foo.Bar")

  void OutputTestLogLines() {
    LOG_TRACE("Trace class line");
    LOG_DEBUG("Debug class line");
    LOG_INFO("Info class line");
    LOG_WARN("Warn class line");
    LOG_ERROR("Error class line");
    LOG_FATAL("Fatal class line");
  }

  void OutputAutoTrace() {
    LOG_AUTO_TRACE();
    LOG_TRACE("Method with auto trace");
  }
};

}  // namespace Foo

TEST(TestLogger, LogFromFreeFunctionFromAnonymousNamespace) {
  OutputTestLogLinesAnonymous();
}

TEST(TestLogger, LogFromClassMethod) {
  Foo::Bar bar;
  bar.OutputTestLogLines();
}

TEST(TestLogger, AutoTrace) {
  Foo::Bar bar;
  bar.OutputAutoTrace();
}

TEST(TestLogger, ChainedOutput) {
  LOG_INFO("Some int: " << 10 << "; some string: "
                        << "str");
}

#if !defined(DISABLE_LOGGER)
TEST(TestLogger, UseLocalLogger) {
  auto logger = blsb::create_logger("Test.Local.Logger");
  LOG_INFOL(logger, "Log with local logger");
}
#endif  // DISABLE_LOGGER

TEST(TestLogger, WriteToLogInLoop) {
  LOG_AUTO_TRACE();
  const auto kIterationCount = 7000;
  for (auto i = 1; i <= kIterationCount; ++i) {
    LOG_TRACE("Logging from loop. Iteration #" << i);
  }
}
