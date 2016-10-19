// Copyright [2016] <Malinovsky Rodion>

#include "util/logger.h"
#if !defined(DISABLE_LOGGER)
#include <fstream>
#include <vector>
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/log/attributes/constant.hpp"
#include "boost/log/expressions.hpp"
#include "boost/log/support/date_time.hpp"
#include "boost/log/utility/setup/common_attributes.hpp"
#include "boost/log/utility/setup/filter_parser.hpp"
#include "boost/log/utility/setup/formatter_parser.hpp"
#include "boost/log/utility/setup/from_stream.hpp"

namespace {

const std::vector<std::string> kSeverityLevelNames = {
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL"};

const auto kSeverityLevelMaxNameSize = 5;

const char* ToString(prj_demo::util::logging::SeverityLevel lvl) {
  const auto names_count = kSeverityLevelNames.size();
  const auto integral_lvl = static_cast<std::size_t>(lvl);
  if (integral_lvl < names_count) {
    return kSeverityLevelNames[integral_lvl].data();
  } else {
    return NULL;
  }
}

}  // namespace

template <typename CharT, typename TraitsT>
std::basic_ostream<CharT, TraitsT>& prj_demo::util::logging::operator<<(
    std::basic_ostream<CharT, TraitsT>& strm,
    prj_demo::util::logging::SeverityLevel lvl) {
  const char* lvl_str = ToString(lvl);
  if (lvl_str) {
    strm << std::setw(kSeverityLevelMaxNameSize) << lvl_str;
  } else {
    strm << static_cast<int>(lvl);
  }
  return strm;
}

template <typename CharT, typename TraitsT>
std::basic_istream<CharT, TraitsT>& prj_demo::util::logging::operator>>(
    std::basic_istream<CharT, TraitsT>& strm,
    prj_demo::util::logging::SeverityLevel& lvl) {
  if (strm.good()) {
    const auto names_count = kSeverityLevelNames.size();
    std::string str;
    strm >> str;
    for (std::size_t i = 0; i < names_count; ++i) {
      if (str == kSeverityLevelNames[i]) {
        lvl = static_cast<prj_demo::util::logging::SeverityLevel>(i);
        return strm;
      }
    }
    strm.setstate(std::ios_base::failbit);
  }
  return strm;
}

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

void InitLogging(std::istream& log_config) {
  boost::log::register_simple_formatter_factory<
      prj_demo::util::logging::SeverityLevel,
      char>("Severity");

  boost::log::register_simple_filter_factory<
      prj_demo::util::logging::SeverityLevel,
      char>("Severity");

  boost::log::register_formatter_factory(
      "TimeStamp", boost::make_shared<TimeStampFormatterFactory>());

  boost::log::add_common_attributes();

  boost::log::init_from_stream(log_config);
}

}  // namespace

prj_demo::util::logging::LoggerClassType prj_demo::util::logging::CreateLogger(
    const char* name) {
  prj_demo::util::logging::LoggerClassType logger;
  logger.add_attribute("Name",
                       boost::log::attributes::constant<std::string>(name));
  return logger;
}

prj_demo::util::logging::LogManager::LogManager(const char* config_file_path) {
  std::ifstream log_config(config_file_path);
  if (!log_config.is_open()) {
    return;
  }
  InitLogging(log_config);
}

prj_demo::util::logging::LogManager::LogManager(const std::string& log_config) {
  std::stringstream sstream(log_config);
  InitLogging(sstream);
}

prj_demo::util::logging::LogManager::~LogManager() {
  Shutdown();
}

void prj_demo::util::logging::LogManager::Shutdown() {
  Flush();
  boost::log::core::get()->remove_all_sinks();
}

void prj_demo::util::logging::LogManager::Flush() {
  boost::log::core::get()->flush();
}

#endif  // DISABLE_LOGGER
