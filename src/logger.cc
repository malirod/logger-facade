#include "logger.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/attributes/constant.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/filter_parser.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>
#include <boost/log/utility/setup/from_stream.hpp>
#include <fstream>

namespace {

// Custom date-time format support
// Allows %TimeStamp(format=\"%Y.%m.%d %H:%M:%S.%f\")% in config file.
class TimeStampFormatterFactory
    : public boost::log::basic_formatter_factory<char,
                                                 boost::posix_time::ptime> {
 public:
  formatter_type create_formatter(const boost::log::attribute_name& name,
                                  const args_map& args) {
    args_map::const_iterator it = args.find("format");
    if (it != args.end()) {
      return boost::log::expressions::stream
             << boost::log::expressions::format_date_time<
                 boost::posix_time::ptime>(
                 boost::log::expressions::attr<boost::posix_time::ptime>(name),
                 it->second);
    } else {
      return boost::log::expressions::stream
             << boost::log::expressions::attr<boost::posix_time::ptime>(name);
    }
  }
};

}  // namespace

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

  boost::log::register_formatter_factory(
      "TimeStamp", boost::make_shared<TimeStampFormatterFactory>());

  boost::log::add_common_attributes();

  boost::log::init_from_stream(log_config);
}

LOGGER_CLASS_TYPE blsb::create_logger(const char* name) {
  LOGGER_CLASS_TYPE logger;
  logger.add_attribute("Name",
                       boost::log::attributes::constant<std::string>(name));
  return logger;
}
