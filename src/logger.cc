#include "logger.h"
#include <boost/log/utility/setup/from_stream.hpp>
#include <fstream>

void blsb::load_log_config(const std::string& path) {
  std::ifstream log_config(path);
  if (log_config.is_open()) {
    boost::log::init_from_stream(log_config);
  }
}
