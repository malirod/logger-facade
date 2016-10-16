#pragma once

#define LOGGER_EMPTY_BODY (void)0;

#if !defined(DISABLE_LOGGER)

#include <boost/log/core/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>

#define LOGGER_CLASS_TYPE boost::log::trivial::logger_type

#define LOGGER_SEVERITY_TYPE_TRACE boost::log::trivial::trace
#define LOGGER_SEVERITY_TYPE_DEBUG boost::log::trivial::debug
#define LOGGER_SEVERITY_TYPE_INFO boost::log::trivial::info
#define LOGGER_SEVERITY_TYPE_WARN boost::log::trivial::warning
#define LOGGER_SEVERITY_TYPE_ERROR boost::log::trivial::error
#define LOGGER_SEVERITY_TYPE_FATAL boost::log::trivial::fatal

#define BLSB_LOG_SCOPE(logger, severity, line, file, function) \
  BOOST_LOG_SEV(logger, severity)                              \
      << boost::log::add_value("Line", line)                   \
      << boost::log::add_value("File", file)                   \
      << boost::log::add_value("Function", function)

namespace blsb {

void init_logging(const std::string& config_file_path);
LOGGER_CLASS_TYPE create_logger(const char* name);

class TraceLogger {
 public:
  TraceLogger(const TraceLogger&) = delete;
  TraceLogger& operator=(const TraceLogger&) = delete;

  TraceLogger(LOGGER_CLASS_TYPE logger,
              std::string message,
              const char* file,
              int line,
              char const* function)
      : logger_(std::move(logger))
      , message_(std::move(message))
      , file_(file)
      , line_(line)
      , function_(function) {
    BLSB_LOG_SCOPE(logger_, LOGGER_SEVERITY_TYPE_TRACE, line_, file_, function_)
        << "ENTER: " << message_;
  }

  ~TraceLogger() {
    BLSB_LOG_SCOPE(logger_, LOGGER_SEVERITY_TYPE_TRACE, line_, file_, function_)
        << "EXIT: " << message_;
  }

 private:
  LOGGER_CLASS_TYPE logger_;
  std::string message_;
  const char* file_;
  int line_;
  const char* function_;
};

}  // namespace blsb

#define INIT_LOGGER(LogConfigPath) blsb::init_logging(LogConfigPath)

#define BLSB_LOG(logger, severity) \
  BLSB_LOG_SCOPE(logger, severity, __LINE__, __FILE__, BOOST_CURRENT_FUNCTION)

#if defined(CUT_OFF_DEBUG_LOG)
#define LOG_TRACEL(logger) LOGGER_EMPTY_BODY
#define LOG_DEBUGL(logger) LOGGER_EMPTY_BODY
#else  // CUT_OFF_DEBUG_LOG
#define LOG_TRACEL(logger) BLSB_LOG(logger, LOGGER_SEVERITY_TYPE_TRACE)
#define LOG_DEBUGL(logger) BLSB_LOG(logger, LOGGER_SEVERITY_TYPE_DEBUG)
#endif  // CUT_OFF_DEBUG_LOG

#define LOG_INFOL(logger) BLSB_LOG(logger, LOGGER_SEVERITY_TYPE_INFO)
#define LOG_WARNL(logger) BLSB_LOG(logger, LOGGER_SEVERITY_TYPE_WARN)
#define LOG_ERRORL(logger) BLSB_LOG(logger, LOGGER_SEVERITY_TYPE_ERROR)
#define LOG_FATALL(logger) BLSB_LOG(logger, LOGGER_SEVERITY_TYPE_FATAL)

#define DECLARE_GET_LOGGER(LoggerName)                    \
  LOGGER_CLASS_TYPE& GetLogger() {                        \
    static auto logger = blsb::create_logger(LoggerName); \
    return logger;                                        \
  }

#define DECLARE_GLOBAL_GET_LOGGER(LoggerName) \
  namespace {                                 \
  DECLARE_GET_LOGGER(LoggerName)              \
  }  // namespace

#if defined(CUT_OFF_DEBUG_LOG)
#define LOG_TRACE() LOGGER_EMPTY_BODY
#define LOG_DEBUG() LOGGER_EMPTY_BODY
#else  // CUT_OFF_DEBUG_LOG
#define LOG_TRACE() LOG_TRACEL(GetLogger())
#define LOG_DEBUG() LOG_DEBUGL(GetLogger())
#endif  // CUT_OFF_DEBUG_LOG

#define LOG_INFO() LOG_INFOL(GetLogger())
#define LOG_WARN() LOG_WARNL(GetLogger())
#define LOG_ERROR() LOG_ERRORL(GetLogger())
#define LOG_FATAL() LOG_FATALL(GetLogger())

#define LOG_AUTO_TRACEL(logger, message) \
  blsb::TraceLogger auto_trace_logger__( \
      logger, message, __FILE__, __LINE__, BOOST_CURRENT_FUNCTION);
#define LOG_AUTO_TRACE() LOG_AUTO_TRACEL(GetLogger(), BOOST_CURRENT_FUNCTION);

#else  // DISABLE_LOGGER

#define INIT_LOGGER(LogConfigPath) LOGGER_EMPTY_BODY

#define BLSB_LOG(logger, severity) LOGGER_EMPTY_BODY

#define LOG_TRACEL(logger) LOGGER_EMPTY_BODY
#define LOG_DEBUGL(logger) LOGGER_EMPTY_BODY
#define LOG_INFOL(logger) LOGGER_EMPTY_BODY
#define LOG_WARNL(logger) LOGGER_EMPTY_BODY
#define LOG_ERRORL(logger) LOGGER_EMPTY_BODY
#define LOG_FATALL(logger) LOGGER_EMPTY_BODY

#define DECLARE_GET_LOGGER(LoggerName) LOGGER_EMPTY_BODY
#define DECLARE_GLOBAL_GET_LOGGER(LoggerName) LOGGER_EMPTY_BODY

#define LOG_TRACE() LOGGER_EMPTY_BODY
#define LOG_DEBUG() LOGGER_EMPTY_BODY
#define LOG_INFO(l) LOGGER_EMPTY_BODY
#define LOG_WARN(l) LOGGER_EMPTY_BODY
#define LOG_ERROR() LOGGER_EMPTY_BODY
#define LOG_FATAL() LOGGER_EMPTY_BODY

#define LOG_AUTO_TRACEL(logger, message) LOGGER_EMPTY_BODY
#define LOG_AUTO_TRACE() LOGGER_EMPTY_BODY

#endif  // DISABLE_LOGGER
