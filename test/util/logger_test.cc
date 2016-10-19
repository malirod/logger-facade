// Copyright [2016] <Malinovsky Rodion>

#include "util/logger.h"
#include <cstdio>
#include <fstream>
#include "gtest/gtest.h"

DECLARE_GLOBAL_GET_LOGGER("LoggerTest.Global")

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

bool Contains(const std::string& str, const std::string& substr) {
  return str.find(substr) != std::string::npos;
}

const char* kLogConfigTemplate = R"(
[Core]
DisableLogging="false"

[Sinks.LogFile]
Filter="%Severity% >= TRACE"

Destination="TextFile"

Asynchronous="true"

AutoFlush="false"

Format="[%Name%][%Severity%]:%Message%"

# Target directory in which rotated files will be stored.
# If not set, then rotation rules will not work as expected
Target="./"

# FileName pattern to use. %N is a counter for files.
FileName="$FILE_NAME"

# Matching used so that only files matching FileName pattern are deleted.
ScanForFiles="Matching"
)";

const char* kLogFileName = "debug-test.log";
const char* kTestConfigFileName = "logger-test.cfg";

#if !defined(DISABLE_LOGGER)

class TestLogger : public ::testing::Test {
 public:
  TestLogger() {}

  ~TestLogger() {
    const auto error = std::remove(kLogFileName);
    EXPECT_EQ(error, 0);
  }

  std::string GetLogOutput() {
    FLUSH_LOGGER();
    std::ifstream ifs(kLogFileName);
    // assert is better, but can't assert in no-void functions
    EXPECT_TRUE(ifs.is_open());
    return std::string(std::istreambuf_iterator<char>(ifs),
                       std::istreambuf_iterator<char>());
  }

  prj_demo::util::logging::LogManager log_manager_{
      ReplaceString(kLogConfigTemplate, "$FILE_NAME", kLogFileName)};
};

#endif  // DISABLE_LOGGER

void OutputTestLogLines() {
  LOG_TRACE("Trace global line");
  LOG_DEBUG("Debug global line");
  // LOG_INFO("Info global line");
  // LOG_WARN("Warn global line");
  // LOG_ERROR("Error global line");
  // LOG_FATAL("Fatal global line");
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

#if !defined(DISABLE_LOGGER)

TEST_F(TestLogger, LogFromFreeFunctionFromAnonymousNamespace) {
  OutputTestLogLines();
  const auto log_content = GetLogOutput();
  ASSERT_TRUE(Contains(log_content, "Trace global line"));
  ASSERT_TRUE(Contains(log_content, "Debug global line"));
}

/*
TEST_F(TestLogger, LogFromClassMethod) {
  Foo::Bar bar;
  bar.OutputTestLogLines();
  const auto log_content = GetLogOutput();
  ASSERT_TRUE(Contains(log_content, "[Foo.Bar][TRACE]:Trace class line"));
  ASSERT_TRUE(Contains(log_content, "[Foo.Bar][DEBUG]:Debug class line"));
  ASSERT_TRUE(Contains(log_content, "[Foo.Bar][ INFO]:Info class line"));
  ASSERT_TRUE(Contains(log_content, "[Foo.Bar][ WARN]:Warn class line"));
  ASSERT_TRUE(Contains(log_content, "[Foo.Bar][ERROR]:Error class line"));
  ASSERT_TRUE(Contains(log_content, "[Foo.Bar][FATAL]:Fatal class line"));
}

TEST_F(TestLogger, AutoTrace) {
  Foo::Bar bar;
  bar.OutputAutoTrace();
  const auto log_content = GetLogOutput();
  // Trace output is compiler specific due to __func__ usage
  ASSERT_TRUE(Contains(log_content, "[Foo.Bar][TRACE]:ENTER"));
  ASSERT_TRUE(Contains(log_content, "[Foo.Bar][TRACE]:EXIT"));
  ASSERT_TRUE(Contains(log_content, "OutputAutoTrace"));
}

TEST_F(TestLogger, ChainedOutput) {
  LOG_INFO("Some int: " << 10 << "; some string: "
                        << "str");
  const auto log_content = GetLogOutput();
  ASSERT_TRUE(Contains(log_content, "Some int: 10; some string: str"));
}

TEST_F(TestLogger, UseLocalLogger) {
  auto logger = prj_demo::util::logging::CreateLogger("Test.Local.Logger");
  LOG_INFOL(logger, "Log with local logger");
  const auto log_content = GetLogOutput();
  ASSERT_TRUE(Contains(log_content,
                       "[Test.Local.Logger][ INFO]:Log with local logger"));
}

TEST_F(TestLogger, WriteToLogInLoop) {
  LOG_AUTO_TRACE();
  const auto kIterationCount = 11000;
  for (auto i = 1; i <= kIterationCount; ++i) {
    LOG_TRACE("Logging from loop. Iteration #" << i);
  }
  const auto log_content = GetLogOutput();
  ASSERT_TRUE(Contains(log_content, "Logging from loop. Iteration #11000"));
}

TEST_F(TestLogger, InitFromFileNameAndFlushWhenOutOfScope) {
  std::ofstream ofs(kTestConfigFileName);
  ofs << ReplaceString(kLogConfigTemplate, "$FILE_NAME", kLogFileName);
  ofs.close();
  std::ifstream config_file(kTestConfigFileName);
  ASSERT_TRUE(config_file.good());
  {
    INIT_LOGGER(kTestConfigFileName);
    OutputTestLogLines();
  }
  std::ifstream debug_log_file(kLogFileName);
  ASSERT_TRUE(debug_log_file.good());

  const auto log_content = GetLogOutput();

  ASSERT_TRUE(
      Contains(log_content, "[LoggerTest.Global][TRACE]:Trace global line"));
  ASSERT_TRUE(
      Contains(log_content, "[LoggerTest.Global][DEBUG]:Debug global line"));
  ASSERT_TRUE(
      Contains(log_content, "[LoggerTest.Global][ INFO]:Info global line"));
  ASSERT_TRUE(
      Contains(log_content, "[LoggerTest.Global][ WARN]:Warn global line"));
  ASSERT_TRUE(
      Contains(log_content, "[LoggerTest.Global][ERROR]:Error global line"));
  ASSERT_TRUE(
      Contains(log_content, "[LoggerTest.Global][FATAL]:Fatal global line"));

  const auto error = std::remove(kTestConfigFileName);
  EXPECT_EQ(error, 0);
}
*/

#else  // DISABLE_LOGGER

TEST(TestLogger, NoOutputWhenLoggingIsDisabled) {
  std::ofstream ofs(kTestConfigFileName);
  ofs << ReplaceString(kLogConfigTemplate, "$FILE_NAME", kLogFileName);
  ofs.close();
  std::ifstream config_file(kTestConfigFileName);
  ASSERT_TRUE(config_file.good());
  {
    INIT_LOGGER(kTestConfigFileName);
    OutputTestLogLines();
    FLUSH_LOGGER();
  }
  std::ifstream debug_log_file(kLogFileName);
  ASSERT_FALSE(debug_log_file.good());

  const auto error = std::remove(kTestConfigFileName);
  EXPECT_EQ(error, 0);
}

#endif  // DISABLE_LOGGER
