// Copyright [2016] <Malinovsky Rodion>

#pragma once

#define DOWHILE_NOTHING() \
  do {                    \
  } while (0)

#if defined(DISABLE_LOGGER)

#define INIT_LOGGER(log_config_path) DOWHILE_NOTHING()
#define FLUSH_LOGGER() DOWHILE_NOTHING()

#define BLSB_LOG(logger, severity, message) DOWHILE_NOTHING()

#define LOG_TRACEL(logger, message) DOWHILE_NOTHING()
#define LOG_DEBUGL(logger, message) DOWHILE_NOTHING()
#define LOG_INFOL(logger, message) DOWHILE_NOTHING()
#define LOG_WARNL(logger, message) DOWHILE_NOTHING()
#define LOG_ERRORL(logger, message) DOWHILE_NOTHING()
#define LOG_FATALL(logger, message) DOWHILE_NOTHING()

#define DECLARE_GET_LOGGER(logger_name)
#define DECLARE_GLOBAL_GET_LOGGER(logger_name)

#define LOG_TRACE(message) DOWHILE_NOTHING()
#define LOG_DEBUG(message) DOWHILE_NOTHING()
#define LOG_INFO(message) DOWHILE_NOTHING()
#define LOG_WARN(message) DOWHILE_NOTHING()
#define LOG_ERROR(message) DOWHILE_NOTHING()
#define LOG_FATAL(message) DOWHILE_NOTHING()

#define LOG_AUTO_TRACEL(logger, message) DOWHILE_NOTHING()
#define LOG_AUTO_TRACE() DOWHILE_NOTHING()

#else  // DISABLE_LOGGER

#include <string>
#include <utility>
#include "boost/log/core/core.hpp"
#include "boost/log/sources/severity_logger.hpp"
#include "boost/log/utility/manipulators/add_value.hpp"

#define BLSB_LOG_SCOPE(logger, severity, line, file, function, message) \
  BOOST_LOG_SEV(logger, severity) << message;

namespace prj_demo {
namespace util {
namespace logging {

enum SeverityLevel { lvlTRACE, lvlDEBUG, lvlINFO, lvlWARN, lvlERROR, lvlFATAL };

using LoggerClassType = boost::log::sources::severity_logger_mt<SeverityLevel>;

template <typename CharT, typename TraitsT>
std::basic_ostream<CharT, TraitsT>& operator<<(
    std::basic_ostream<CharT, TraitsT>& strm, SeverityLevel lvl);

template <typename CharT, typename TraitsT>
std::basic_istream<CharT, TraitsT>& operator>>(
    std::basic_istream<CharT, TraitsT>& strm, SeverityLevel& lvl);

LoggerClassType CreateLogger(const char* name);

class TraceLogger {
 public:
  TraceLogger(const TraceLogger&) = delete;
  TraceLogger& operator=(const TraceLogger&) = delete;

  TraceLogger(LoggerClassType logger,
              const std::string& message,
              const char* file,
              int line,
              char const* function)
      : logger_(std::move(logger))
      , message_(message)
      , file_(file)
      , line_(line)
      , function_(function) {
    BLSB_LOG_SCOPE(logger_,
                   prj_demo::util::logging::lvlTRACE,
                   line_,
                   file_,
                   function_,
                   "ENTER: " << message_);
  }

  ~TraceLogger() {
    BLSB_LOG_SCOPE(logger_,
                   prj_demo::util::logging::lvlTRACE,
                   line_,
                   file_,
                   function_,
                   "EXIT: " << message_);
  }

 private:
  LoggerClassType logger_;
  std::string message_;
  const char* file_;
  int line_;
  const char* function_;
};

class LogManager {
 public:
  explicit LogManager(const char* config_file_path);

  explicit LogManager(const std::string& log_config);

  ~LogManager();

  static void Flush();

  static void Shutdown();

  LogManager(const LogManager&) = delete;
  LogManager& operator=(const LogManager&) = delete;
};

}  // namespace logging
}  // namespace util
}  // namespace prj_demo

#define INIT_LOGGER(log_config_path) \
  prj_demo::util::logging::LogManager log_manager__(log_config_path)

#define FLUSH_LOGGER() prj_demo::util::logging::LogManager::Flush()

#define BLSB_LOG(logger, severity, message) \
  BLSB_LOG_SCOPE(                           \
      logger, severity, __LINE__, __FILE__, BOOST_CURRENT_FUNCTION, message)

#if defined(CUT_OFF_DEBUG_LOG)
#define LOG_TRACEL(logger, message) DOWHILE_NOTHING()
#define LOG_DEBUGL(logger, message) DOWHILE_NOTHING()
#else  // CUT_OFF_DEBUG_LOG
#define LOG_TRACEL(logger, message) \
  BLSB_LOG(logger, prj_demo::util::logging::lvlTRACE, message)
#define LOG_DEBUGL(logger, message) \
  BLSB_LOG(logger, prj_demo::util::logging::lvlDEBUG, message)
#endif  // CUT_OFF_DEBUG_LOG

#define LOG_INFOL(logger, message) \
  BLSB_LOG(logger, prj_demo::util::logging::lvlINFO, message)
#define LOG_WARNL(logger, message) \
  BLSB_LOG(logger, prj_demo::util::logging::lvlWARN, message)
#define LOG_ERRORL(logger, message) \
  BLSB_LOG(logger, prj_demo::util::logging::lvlERROR, message)
#define LOG_FATALL(logger, message) \
  BLSB_LOG(logger, prj_demo::util::logging::lvlFATAL, message)

#define DECLARE_GET_LOGGER(logger_name)                                      \
  static prj_demo::util::logging::LoggerClassType& GetLogger() {             \
    static auto logger = prj_demo::util::logging::CreateLogger(logger_name); \
    return logger;                                                           \
  }

#define DECLARE_GLOBAL_GET_LOGGER(logger_name) \
  namespace {                                  \
  DECLARE_GET_LOGGER(logger_name)              \
  }  // namespace

#if defined(CUT_OFF_DEBUG_LOG)
#define LOG_TRACE(message) DOWHILE_NOTHING()
#define LOG_DEBUG(message) DOWHILE_NOTHING()
#else  // CUT_OFF_DEBUG_LOG
#define LOG_TRACE(message) LOG_TRACEL(GetLogger(), message)
#define LOG_DEBUG(message) LOG_DEBUGL(GetLogger(), message)
#endif  // CUT_OFF_DEBUG_LOG

#define LOG_INFO(message) LOG_INFOL(GetLogger(), message)
#define LOG_WARN(message) LOG_WARNL(GetLogger(), message)
#define LOG_ERROR(message) LOG_ERRORL(GetLogger(), message)
#define LOG_FATAL(message) LOG_FATALL(GetLogger(), message)

#define LOG_AUTO_TRACEL(logger, message)                    \
  prj_demo::util::logging::TraceLogger auto_trace_logger__( \
      logger, message, __FILE__, __LINE__, BOOST_CURRENT_FUNCTION);

#define LOG_AUTO_TRACE() LOG_AUTO_TRACEL(GetLogger(), BOOST_CURRENT_FUNCTION);

#endif  // DISABLE_LOGGER
