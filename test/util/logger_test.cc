// Copyright [2016] <Malinovsky Rodion>

#include "util/logger.h"
#include <cstdio>
#include <fstream>
#include "gtest/gtest.h"

DECLARE_GLOBAL_GET_LOGGER("Logger.Global")

namespace {

std::string ReplaceString(std::string subject,
                          const std::string& search,
                          const std::string& replace) {
  std::size_t pos = 0;
  while ((pos = subject.find(search, pos)) != std::string::npos) {
    subject.replace(pos, search.length(), replace);
    pos += replace.length();
  }
  return subject;
}

#if defined(USE_BOOST_LOGGER)

const char* kLogConfigTemplate =
    R"([Core]
DisableLogging="false"

[Sinks.LogFile]
Filter="%Severity% >= TRACE"

Destination="TextFile"

Asynchronous="true"

AutoFlush="false"

Format="%TimeStamp(format=\"%H:%M:%S.%f\")% [%Name%][%Severity%]:%Message%"

# Target directory in which rotated files will be stored.
# If not set, then rotation rules will not work as expected
Target="./"

# FileName pattern to use. %N is a counter for files.
FileName="$FILE_NAME"

# Matching used so that only files matching FileName pattern are deleted.
ScanForFiles="Matching"
)";

#else

const char* kLogConfigTemplate =
    R"(# Define a file async appender named "ASYNCFILE"
log4cplus.appender.ASYNCFILE=log4cplus::AsyncAppender
log4cplus.appender.ASYNCFILE.Appender=log4cplus::RollingFileAppender
log4cplus.appender.ASYNCFILE.Appender.MaxBackupIndex=2
log4cplus.appender.ASYNCFILE.Appender.MaxFileSize=10MB
log4cplus.appender.ASYNCFILE.Appender.File=$FILE_NAME
log4cplus.appender.ASYNCFILE.Appender.layout=log4cplus::PatternLayout
log4cplus.appender.ASYNCFILE.Appender.layout.ConversionPattern=[%c][%5p]:%m%n

#log4cplus.appender.ROLLFILE=log4cplus::RollingFileAppender
#log4cplus.appender.ROLLFILE.MaxBackupIndex=2
#log4cplus.appender.ROLLFILE.MaxFileSize=10MB
#log4cplus.appender.ROLLFILE.File=$FILE_NAME
#log4cplus.appender.ROLLFILE.layout=log4cplus::PatternLayout
#log4cplus.appender.ROLLFILE.layout.ConversionPattern=[%c][%5p]:%m%n

# Define the root logger
log4cplus.rootLogger=TRACE, ASYNCFILE
#log4cplus.rootLogger=TRACE, ROLLFILE
)";

#endif

const char* kLogFileName = "debug-test.log";
const char* kTestConfigFileName = "logger-test.cfg";

#if !defined(DISABLE_LOGGER)

std::string GetLogOutput() {
  SHUTDOWN_LOGGER();
  std::ifstream ifs(kLogFileName);
  // assert is better, but can't assert in no-void functions
  EXPECT_TRUE(ifs.is_open());
  return std::string(std::istreambuf_iterator<char>(ifs),
                     std::istreambuf_iterator<char>());
}

template <typename T>
void InitLoggerAndRunTest(T action) {
  std::stringstream log_config(
      ReplaceString(kLogConfigTemplate, "$FILE_NAME", kLogFileName));
  INIT_LOGGER(log_config);
  action();
  const auto error = std::remove(kLogFileName);
  ASSERT_EQ(error, 0);
}

#endif  // DISABLE_LOGGER

void OutputTestLogLines() {
  LOG_TRACE("Trace global line");
  LOG_DEBUG("Debug global line");
  LOG_INFO("Info global line");
  LOG_WARN("Warn global line");
  LOG_ERROR("Error global line");
  LOG_FATAL("Fatal global line");
}

void TestContains(const std::string& log_content, const std::string& substr) {
  ASSERT_TRUE(log_content.find(substr) != std::string::npos)
      << "Log doesn't contain line: " << substr << std::endl
      << "Log content:" << std::endl
      << log_content << std::endl;
}

}  // namespace

namespace Foo {

class Bar {
 public:
  DECLARE_GET_LOGGER("Foo.Bar")

  static void OutputTestLogLines() {
    LOG_TRACE("Trace class line");
    LOG_DEBUG("Debug class line");
    LOG_INFO("Info class line");
    LOG_WARN("Warn class line");
    LOG_ERROR("Error class line");
    LOG_FATAL("Fatal class line");
  }

  static void OutputAutoTrace() {
    LOG_AUTO_TRACE();
    LOG_TRACE("Method with auto trace");
  }
};

}  // namespace Foo

#if defined(DISABLE_LOGGER)

TEST(TestLogger, NoOutputWhenLoggingIsDisabled) {
  std::ofstream ofs(kTestConfigFileName);
  ofs << ReplaceString(kLogConfigTemplate, "$FILE_NAME", kLogFileName);
  ofs.close();
  std::ifstream config_file(kTestConfigFileName);
  ASSERT_TRUE(config_file.good());
  {
    INIT_LOGGER(kTestConfigFileName);
    OutputTestLogLines();
  }
  const auto error = std::remove(kTestConfigFileName);
  EXPECT_EQ(error, 0);

  std::ifstream debug_log_file(kLogFileName);
  ASSERT_FALSE(debug_log_file.good());
}

#else  // DISABLE_LOGGER

TEST(TestLogger, LogFromFreeFunctionFromAnonymousNamespace) {
  const auto action = []() {
    OutputTestLogLines();
    const auto log_content = GetLogOutput();
    TestContains(log_content, "[Logger.Global][TRACE]:Trace global line");
    TestContains(log_content, "[Logger.Global][DEBUG]:Debug global line");
    TestContains(log_content, "[Logger.Global][ INFO]:Info global line");
    TestContains(log_content, "[Logger.Global][ WARN]:Warn global line");
    TestContains(log_content, "[Logger.Global][ERROR]:Error global line");
    TestContains(log_content, "[Logger.Global][FATAL]:Fatal global line");
  };

  InitLoggerAndRunTest(action);
}

TEST(TestLogger, LogFromClassMethod) {
  const auto action = []() {
    Foo::Bar bar;
    bar.OutputTestLogLines();
    const auto log_content = GetLogOutput();
    TestContains(log_content, "[Foo.Bar][TRACE]:Trace class line");
    TestContains(log_content, "[Foo.Bar][DEBUG]:Debug class line");
    TestContains(log_content, "[Foo.Bar][ INFO]:Info class line");
    TestContains(log_content, "[Foo.Bar][ WARN]:Warn class line");
    TestContains(log_content, "[Foo.Bar][ERROR]:Error class line");
    TestContains(log_content, "[Foo.Bar][FATAL]:Fatal class line");
  };

  InitLoggerAndRunTest(action);
}

TEST(TestLogger, AutoTrace) {
  const auto action = []() {
    Foo::Bar bar;
    bar.OutputAutoTrace();
    const auto log_content = GetLogOutput();
    // Trace output is compiler specific due to __func__ usage
    TestContains(log_content, "[Foo.Bar][TRACE]:ENTER");
    TestContains(log_content, "[Foo.Bar][TRACE]:EXIT");
    TestContains(log_content, "OutputAutoTrace");
  };

  InitLoggerAndRunTest(action);
}

TEST(TestLogger, ChainedOutput) {
  const auto action = []() {
    LOG_INFO("Some int: " << 10 << "; some string: "
                          << "str");
    const auto log_content = GetLogOutput();
    TestContains(log_content, "Some int: 10; some string: str");
  };

  InitLoggerAndRunTest(action);
}

TEST(TestLogger, UseLocalLogger) {
  const auto action = []() {
    IMPLEMENT_STATIC_LOGGER("Logger.Local");
    LOG_INFOL(logger, "Log with local logger");
    const auto log_content = GetLogOutput();
    TestContains(log_content, "[Logger.Local][ INFO]:Log with local logger");
  };

  InitLoggerAndRunTest(action);
}

TEST(TestLogger, WriteToLogInLoop) {
  const auto action = []() {
    const auto kIterationCount = 11000;
    for (auto i = 1; i <= kIterationCount; ++i) {
      LOG_TRACE("Logging from loop. Iteration #" << i);
    }
    const auto log_content = GetLogOutput();
    TestContains(log_content, "Logging from loop. Iteration #11000");
  };

  InitLoggerAndRunTest(action);
}

TEST(TestLogger, InitFromFileNameAndFlushWhenOutOfScope) {
  std::ofstream ofs(kTestConfigFileName);
  ofs << ReplaceString(kLogConfigTemplate, "$FILE_NAME", kLogFileName);
  ofs.close();
  std::ifstream config_file(kTestConfigFileName);
  ASSERT_TRUE(config_file.good());
  {
    INIT_LOGGER(kTestConfigFileName);
    OutputTestLogLines();
  }
  const auto delete_cfg_error = std::remove(kTestConfigFileName);
  EXPECT_EQ(delete_cfg_error, 0);

  std::ifstream debug_log_file(kLogFileName);
  ASSERT_TRUE(debug_log_file.good());
  debug_log_file.close();

  const auto log_content = GetLogOutput();

  TestContains(log_content, "[Logger.Global][TRACE]:Trace global line");
  TestContains(log_content, "[Logger.Global][DEBUG]:Debug global line");
  TestContains(log_content, "[Logger.Global][ INFO]:Info global line");
  TestContains(log_content, "[Logger.Global][ WARN]:Warn global line");
  TestContains(log_content, "[Logger.Global][ERROR]:Error global line");
  TestContains(log_content, "[Logger.Global][FATAL]:Fatal global line");

  const auto delete_log_error = std::remove(kLogFileName);
  EXPECT_EQ(delete_log_error, 0);
}

#endif  // DISABLE_LOGGER
