#include "logger.h"
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/filter_parser.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>
#include <boost/log/utility/setup/from_stream.hpp>
#include <fstream>

void blsb::init_logging(const std::string& config_file_path) {
  std::ifstream log_config(config_file_path);
  if (!log_config.is_open()) {
    return;
  }

  boost::log::register_simple_formatter_factory<
      boost::log::trivial::severity_level,
      char>("Severity");

  boost::log::register_simple_filter_factory<
      boost::log::trivial::severity_level,
      char>("Severity");

  boost::log::init_from_stream(log_config);

  boost::log::add_common_attributes();
}
