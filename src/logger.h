#pragma once

#include <boost/log/core/core.hpp>
#include <boost/log/trivial.hpp>

namespace blsb {

using LoggerType = boost::log::trivial::logger_type;

void load_log_config();

}  // namespace blsb

#define BLSB_LOG(logger, severity) BOOST_LOG_SEV(logger, severity)

#define LOG_TRACE(logger) BLSB_LOG(logger, boost::log::trivial::trace)
#define LOG_INFO(logger) BLSB_LOG(logger, boost::log::trivial::info)
#define LOG_DEBUG(logger) BLSB_LOG(logger, boost::log::trivial::debug)
#define LOG_WARNING(logger) BLSB_LOG(logger, boost::log::trivial::warning)
#define LOG_ERROR(logger) BLSB_LOG(logger, boost::log::trivial::error)
#define LOG_FATAL(logger) BLSB_LOG(logger, boost::log::trivial::fatal)

#define INIT_LOGGER() blsb::load_log_config()
