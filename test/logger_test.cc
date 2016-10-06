#include "logger.h"
#include "gtest/gtest.h"

DECLARE_GLOBAL_GET_LOGGER("LoggerTest.Global")

namespace {

void OutputTestLogLinesAnonymous() {
  LOG_TRACE() << "Trace line";
  LOG_DEBUG() << "Debug line";
  LOG_INFO() << "Info line";
  LOG_WARN() << "Warn line";
  LOG_ERROR() << "Error line";
  LOG_FATAL() << "Fatal line";
}

}  // namespace

namespace Foo {

class Bar {
 public:
  DECLARE_GET_LOGGER("Foo.Bar");

  void OutputTestLogLines() {
    OutputTestLogLinesAnonymous();
  }
};

}  // namespace Foo

TEST(TestLogger, LogFromFreeFunctionFromAnonymousNamespace) {
  OutputTestLogLinesAnonymous();
}

TEST(TestLogger, LogFromClassMethod) {
  Foo::Bar bar;
  bar.OutputTestLogLines();

  BOOST_LOG_TRIVIAL(fatal) << "AAA";
}
