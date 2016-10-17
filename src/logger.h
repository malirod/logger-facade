#pragma once

#define DOWHILE_NOTHING() \
  do {                    \
  } while (0)

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

#define BLSB_LOG_SCOPE(logger, severity, line, file, function, message) \
  BOOST_LOG_SEV(logger, severity)                                       \
      << boost::log::add_value("Line", line)                            \
      << boost::log::add_value("File", file)                            \
      << boost::log::add_value("Function", function) << message

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
    BLSB_LOG_SCOPE(logger_,
                   LOGGER_SEVERITY_TYPE_TRACE,
                   line_,
                   file_,
                   function_,
                   "ENTER: " << message_);
  }

  ~TraceLogger() {
    BLSB_LOG_SCOPE(logger_,
                   LOGGER_SEVERITY_TYPE_TRACE,
                   line_,
                   file_,
                   function_,
                   "EXIT: " << message_);
  }

 private:
  LOGGER_CLASS_TYPE logger_;
  std::string message_;
  const char* file_;
  int line_;
  const char* function_;
};

}  // namespace blsb

#define INIT_LOGGER(log_config_path) blsb::init_logging(log_config_path)

#define BLSB_LOG(logger, severity, message) \
  BLSB_LOG_SCOPE(                           \
      logger, severity, __LINE__, __FILE__, BOOST_CURRENT_FUNCTION, message)

#if defined(CUT_OFF_DEBUG_LOG)
#define LOG_TRACEL(logger, message) DOWHILE_NOTHING()
#define LOG_DEBUGL(logger, message) DOWHILE_NOTHING()
#else  // CUT_OFF_DEBUG_LOG
#define LOG_TRACEL(logger, message) \
  BLSB_LOG(logger, LOGGER_SEVERITY_TYPE_TRACE, message)
#define LOG_DEBUGL(logger, message) \
  BLSB_LOG(logger, LOGGER_SEVERITY_TYPE_DEBUG, message)
#endif  // CUT_OFF_DEBUG_LOG

#define LOG_INFOL(logger, message) \
  BLSB_LOG(logger, LOGGER_SEVERITY_TYPE_INFO, message)
#define LOG_WARNL(logger, message) \
  BLSB_LOG(logger, LOGGER_SEVERITY_TYPE_WARN, message)
#define LOG_ERRORL(logger, message) \
  BLSB_LOG(logger, LOGGER_SEVERITY_TYPE_ERROR, message)
#define LOG_FATALL(logger, message) \
  BLSB_LOG(logger, LOGGER_SEVERITY_TYPE_FATAL, message)

#define DECLARE_GET_LOGGER(logger_name)                    \
  LOGGER_CLASS_TYPE& GetLogger() {                         \
    static auto logger = blsb::create_logger(logger_name); \
    return logger;                                         \
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

#define LOG_AUTO_TRACEL(logger, message) \
  blsb::TraceLogger auto_trace_logger__( \
      logger, message, __FILE__, __LINE__, BOOST_CURRENT_FUNCTION);
#define LOG_AUTO_TRACE() LOG_AUTO_TRACEL(GetLogger(), BOOST_CURRENT_FUNCTION);

#else  // DISABLE_LOGGER

#define INIT_LOGGER(log_config_path) DOWHILE_NOTHING()

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

#endif  // DISABLE_LOGGER
