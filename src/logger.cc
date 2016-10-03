#include "logger.h"
#include <boost/log/core/core.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sources/severity_logger.hpp>

namespace sdllog {

BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int)
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity,
                            "Severity",
                            logging::trivial::severity_level)

BOOST_LOG_GLOBAL_LOGGER_INIT(logger, src::severity_logger_mt) {
  namespace attrs = boost::log::attributes;
  namespace sinks = boost::log::sinks;

  src::severity_logger_mt<boost::log::trivial::severity_level> logger;

  logger.add_attribute("LineID", attrs::counter<unsigned int>(1));
  logger.add_attribute("TimeStamp", attrs::local_clock());

  return logger;
}

}  // namespace sdllog
