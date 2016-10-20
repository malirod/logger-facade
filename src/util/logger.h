// Copyright [2016] <Malinovsky Rodion>

#pragma once

#define DOWHILE_NOTHING() \
  do {                    \
  } while (0)

#if defined(DISABLE_LOGGER)

#define INIT_LOGGER(log_config) DOWHILE_NOTHING()
#define SHUTDOWN_LOGGER() DOWHILE_NOTHING()

#define LOG_TRACEL(logger, message) DOWHILE_NOTHING()
#define LOG_DEBUGL(logger, message) DOWHILE_NOTHING()
#define LOG_INFOL(logger, message) DOWHILE_NOTHING()
#define LOG_WARNL(logger, message) DOWHILE_NOTHING()
#define LOG_ERRORL(logger, message) DOWHILE_NOTHING()
#define LOG_FATALL(logger, message) DOWHILE_NOTHING()

#define IMPLEMENT_STATIC_LOGGER(logger_name) DOWHILE_NOTHING()
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

#if defined(USE_BOOST_LOGGER)

#include <string>
#include <utility>
#include "boost/log/core/core.hpp"
#include "boost/log/sources/severity_logger.hpp"
#include "boost/log/utility/manipulators/add_value.hpp"

#define IMPL_LOGGER_LOG_SCOPE_(                                    \
    logger, severity, line, file, function, message)               \
  do {                                                             \
    BOOST_LOG_SEV(logger, severity)                                \
        << boost::log::add_value("Line", line)                     \
        << boost::log::add_value("File", file)                     \
        << boost::log::add_value("Function", function) << message; \
  } while (0)

#define IMPL_LOGGER_NAMESPACE_ logger_facade::util::logging
#define IMPL_LOGGER_CLASS_TYPE_ \
  boost::log::sources::severity_logger_mt<IMPL_LOGGER_NAMESPACE_::SeverityLevel>

namespace logger_facade {
namespace util {
namespace logging {

enum SeverityLevel { lvlTRACE, lvlDEBUG, lvlINFO, lvlWARN, lvlERROR, lvlFATAL };

template <typename CharT, typename TraitsT>
std::basic_ostream<CharT, TraitsT>& operator<<(
    std::basic_ostream<CharT, TraitsT>& strm, SeverityLevel lvl);

template <typename CharT, typename TraitsT>
std::basic_istream<CharT, TraitsT>& operator>>(
    std::basic_istream<CharT, TraitsT>& strm, SeverityLevel& lvl);

IMPL_LOGGER_CLASS_TYPE_ CreateLogger(const char* name);

class TraceLogger {
 public:
  TraceLogger(const TraceLogger&) = delete;
  TraceLogger(const TraceLogger&&) = delete;
  TraceLogger& operator=(const TraceLogger&) = delete;
  TraceLogger& operator=(const TraceLogger&&) = delete;

  TraceLogger(IMPL_LOGGER_CLASS_TYPE_ logger,
              const std::string& message,
              const char* file,
              int line,
              char const* function)
      : logger_(std::move(logger))
      , message_(message)
      , file_(file)
      , line_(line)
      , function_(function) {
    IMPL_LOGGER_LOG_SCOPE_(
        logger_, lvlTRACE, line_, file_, function_, "ENTER: " << message_);
  }

  ~TraceLogger() {
    IMPL_LOGGER_LOG_SCOPE_(
        logger_, lvlTRACE, line_, file_, function_, "EXIT: " << message_);
  }

 private:
  IMPL_LOGGER_CLASS_TYPE_ logger_;
  std::string message_;
  const char* file_;
  int line_;
  const char* function_;
};

class LogManager {
 public:
  explicit LogManager(const char* config_file_path);

  explicit LogManager(std::istream& log_config);

  ~LogManager();

  static void Shutdown();

  LogManager(const LogManager&) = delete;
  LogManager(const LogManager&&) = delete;
  LogManager& operator=(const LogManager&) = delete;
  LogManager& operator=(const LogManager&&) = delete;
};

}  // namespace logging
}  // namespace util
}  // namespace logger_facade

#define INIT_LOGGER(log_config) \
  IMPL_LOGGER_NAMESPACE_::LogManager log_manager__(log_config)
#define SHUTDOWN_LOGGER() IMPL_LOGGER_NAMESPACE_::LogManager::Shutdown()

#define IMPL_LOGGER_LOG_(logger, severity, message) \
  IMPL_LOGGER_LOG_SCOPE_(                           \
      logger, severity, __LINE__, __FILE__, BOOST_CURRENT_FUNCTION, message)

// Strip off log lines lower or qeual DEBUG in DEBUG mode
#if defined(CUT_OFF_DEBUG_LOG)
#define LOG_TRACEL(logger, message) DOWHILE_NOTHING()
#define LOG_DEBUGL(logger, message) DOWHILE_NOTHING()
#else  // CUT_OFF_DEBUG_LOG
#define LOG_TRACEL(logger, message) \
  IMPL_LOGGER_LOG_(logger, IMPL_LOGGER_NAMESPACE_::lvlTRACE, message)
#define LOG_DEBUGL(logger, message) \
  IMPL_LOGGER_LOG_(logger, IMPL_LOGGER_NAMESPACE_::lvlDEBUG, message)
#endif  // CUT_OFF_DEBUG_LOG

#define LOG_INFOL(logger, message) \
  IMPL_LOGGER_LOG_(logger, IMPL_LOGGER_NAMESPACE_::lvlINFO, message)
#define LOG_WARNL(logger, message) \
  IMPL_LOGGER_LOG_(logger, IMPL_LOGGER_NAMESPACE_::lvlWARN, message)
#define LOG_ERRORL(logger, message) \
  IMPL_LOGGER_LOG_(logger, IMPL_LOGGER_NAMESPACE_::lvlERROR, message)
#define LOG_FATALL(logger, message) \
  IMPL_LOGGER_LOG_(logger, IMPL_LOGGER_NAMESPACE_::lvlFATAL, message)

#define IMPLEMENT_STATIC_LOGGER(logger_name) \
  static auto logger = IMPL_LOGGER_NAMESPACE_::CreateLogger(logger_name)

#define DECLARE_GET_LOGGER(logger_name)         \
  static IMPL_LOGGER_CLASS_TYPE_& GetLogger() { \
    IMPLEMENT_STATIC_LOGGER(logger_name);       \
    return logger;                              \
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

#define LOG_AUTO_TRACEL(logger, message)                   \
  IMPL_LOGGER_NAMESPACE_::TraceLogger auto_trace_logger__( \
      logger, message, __FILE__, __LINE__, __func__);

#define LOG_AUTO_TRACE() LOG_AUTO_TRACEL(GetLogger(), __func__);

#elif defined(USE_LOG4CPLUS_LOGGER)  // USE_LOG4CPLUS_LOGGER

// Strip off log lines lower or qeual DEBUG in DEBUG mode
#if defined(CUT_OFF_DEBUG_LOG)
#define LOG4CPLUS_DISABLE_DEBUG
#endif  // CUT_OFF_DEBUG_LOG

#include "log4cplus/logger.h"
#include "log4cplus/loggingmacros.h"

#define IMPL_LOGGER_CLASS_TYPE_ log4cplus::Logger
#define IMPL_LOGGER_NAMESPACE_ logger_facade::util::logging

namespace logger_facade {
namespace util {
namespace logging {

class LogManager {
 public:
  explicit LogManager(const char* config_file_path);

  explicit LogManager(std::istream& log_config);

  ~LogManager();

  static void Shutdown();

  LogManager(const LogManager&) = delete;
  LogManager(const LogManager&&) = delete;
  LogManager& operator=(const LogManager&) = delete;
  LogManager& operator=(const LogManager&&) = delete;
};

}  // namespace logging
}  // namespace util
}  // namespace logger_facade

#define INIT_LOGGER(log_config) \
  IMPL_LOGGER_NAMESPACE_::LogManager log_manager__(log_config)
#define SHUTDOWN_LOGGER() IMPL_LOGGER_NAMESPACE_::LogManager::Shutdown();

#define LOG_TRACEL(logger, message) \
  LOG4CPLUS_TRACE(logger, LOG4CPLUS_TEXT(message))
#define LOG_DEBUGL(logger, message) \
  LOG4CPLUS_DEBUG(logger, LOG4CPLUS_TEXT(message))
#define LOG_INFOL(logger, message) \
  LOG4CPLUS_INFO(logger, LOG4CPLUS_TEXT(message))
#define LOG_WARNL(logger, message) \
  LOG4CPLUS_WARN(logger, LOG4CPLUS_TEXT(message))
#define LOG_ERRORL(logger, message) \
  LOG4CPLUS_ERROR(logger, LOG4CPLUS_TEXT(message))
#define LOG_FATALL(logger, message) \
  LOG4CPLUS_FATAL(logger, LOG4CPLUS_TEXT(message))

#define IMPLEMENT_STATIC_LOGGER(logger_name) \
  static auto logger =                       \
      IMPL_LOGGER_CLASS_TYPE_::getInstance(LOG4CPLUS_TEXT(logger_name))

#define DECLARE_GET_LOGGER(logger_name)         \
  static IMPL_LOGGER_CLASS_TYPE_& GetLogger() { \
    IMPLEMENT_STATIC_LOGGER(logger_name);       \
    return logger;                              \
  }

#define DECLARE_GLOBAL_GET_LOGGER(logger_name) \
  namespace {                                  \
  DECLARE_GET_LOGGER(logger_name)              \
  }  // namespace

#define LOG_TRACE(message) LOG_TRACEL(GetLogger(), message)
#define LOG_DEBUG(message) LOG_DEBUGL(GetLogger(), message)
#define LOG_INFO(message) LOG_INFOL(GetLogger(), message)
#define LOG_WARN(message) LOG_WARNL(GetLogger(), message)
#define LOG_ERROR(message) LOG_ERRORL(GetLogger(), message)
#define LOG_FATAL(message) LOG_FATALL(GetLogger(), message)

#define LOG_AUTO_TRACEL(logger, message) LOG4CPLUS_TRACE_METHOD(logger, message)
#define LOG_AUTO_TRACE() LOG_AUTO_TRACEL(GetLogger(), LOG4CPLUS_TEXT(__func__))

#else

#error Logger is not set. [USE_BOOST_LOGGER, USE_LOG4CPLUS_LOGGER]

#endif

#endif
