#pragma once

#include <fstream>

#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/from_stream.hpp>
#include <boost/log/core/core.hpp>
#include <boost/log/sources/logger.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
using LoggerType = src::logger_mt;

#define CREATE_LOCAL_LOGGER(logger) \
  src::logger_mt logger

#define CREATE_GLOBAL_LOGGER(logger) \
namespace { \
  GET_LOCAL_LOGGER(); \
}

#define INIT_LOGGER() \
  std::ifstream log_config("logger.cfg"); \
  if (log_config.is_open()) { \
    logging::init_from_stream(log_config); \
  }

// TODO: Implement logger interface
