#pragma once

#include <fstream>

#include <boost/log/core/core.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/from_stream.hpp>

namespace sdllog {
namespace logging = boost::log;
namespace src = boost::log::sources;
using LoggerType = src::logger_mt;

#define SEVERITY_THRESHOLD logging::trivial::warning

BOOST_LOG_GLOBAL_LOGGER(logger,
                        boost::log::sources::severity_logger_mt<
                            boost::log::trivial::severity_level>)

#define LOG(severity) \
  BOOST_LOG_SEV(logger::get(), boost::log::trivial::severity)

#define LOG_TRACE LOG(trace)
#define LOG_INFO LOG(info)
#define LOG_DEBUG LOG(debug)
#define LOG_WARNING LOG(warning)
#define LOG_ERROR LOG(error)
#define LOG_FATAL LOG(fatal)

#define INIT_LOGGER()                         \
  std::ifstream log_config("logger.cfg");     \
  if (log_config.is_open()) {                 \
    boost::log::init_from_stream(log_config); \
  }

}  // namespace log
