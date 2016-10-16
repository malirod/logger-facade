#include "logger.h"
#include <boost/log/attributes/named_scope.hpp>
#include "gtest/gtest.h"

DECLARE_GLOBAL_GET_LOGGER("LoggerTest.Global")

namespace {

void OutputTestLogLinesAnonymous() {
  LOG_TRACE() << "Trace global line";
  LOG_DEBUG() << "Debug global line";
  LOG_INFO() << "Info global line";
  LOG_WARN() << "Warn global line";
  LOG_ERROR() << "Error global line";
  LOG_FATAL() << "Fatal global line";
}

}  // namespace

namespace Foo {

class Bar {
 public:
  DECLARE_GET_LOGGER("Foo.Bar");

  void OutputTestLogLines() {
    LOG_TRACE() << "Trace class line";
    LOG_DEBUG() << "Debug class line";
    LOG_INFO() << "Info class line";
    LOG_WARN() << "Warn class line";
    LOG_ERROR() << "Error class line";
    LOG_FATAL() << "Fatal class line";
  }

  void OutputAutoTrace() {
    LOG_AUTO_TRACE();
    LOG_TRACE() << "Method with auto trace";
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

TEST(TestLogger, TestAutoTrace) {
  Foo::Bar bar;
  bar.OutputAutoTrace();
}
