[![Build Status](https://travis-ci.org/malirod/logger-facade.svg?branch=master)](https://travis-ci.org/malirod/logger-facade)

# logger-facade
Allows to switch between different loggers keeping the same interface. Supported log4cplus and Boost Log

## Platform

Ubuntu 16.04: Clang 3.8, GCC 5.4

C++11 Standard is used.

## Setup

### Install Boost

#### Use pre-build Boost lib

Use the following repository: https://github.com/malirod/boost-bin

Use appropriate branch. Branch name containts info about used env and options to build Boost.

E.g.

https://github.com/malirod/boost-bin/tree/boost-1.61-ubuntu-16.04-x64-clang-5.8-release-c++11-static-multi

#### Build Boost manually from sources
If pre-build version doesn't fit your needs (OS, compiler etc) then build Boost manually.

Download and unzip to some dir (e.g. `~/libs/boost_1_61_0`) latest stable version from [Boost site](http://www.boost.org/).

Go the unzipped directory and run commands

```
./bootstrap.sh --prefix=./build
./b2 --with=all -j2 install
```

#### Set environment variable
Set env pointing to the boost install dir (in ~/.profile or ~/.bashrc)

`export BOOST_HOME=~/libs/boost_1_61_0/build`

Restart terminal, or reload config with `source ~/.profile` (`source ~/.bashrc`)

### Install log4cplus

#### Build from sources

Can work with either 1.1.x or 1.2.0. Run the following commands in some "libs" directory to setup 1.2.0

```
sudo apt-get -y install autotools-dev automake
wget -O log4cplus-1.2.0.tar.xz http://heanet.dl.sourceforge.net/project/log4cplus/log4cplus-stable/1.2.0/log4cplus-1.2.0.tar.xz
tar xf log4cplus-1.2.0.tar.xz
cd log4cplus-1.2.0
./configure CXX="clang++" --disable-shared --enable-static --prefix=$PWD/build
make -j$(nproc) && make install
```
#### Set environment variable
Set env pointing to the log4cplus install dir (in ~/.profile or ~/.bashrc)

`export LOG4CPLUS_HOME=~/libs/boost_1_61_0/build`

Restart terminal, or reload config with `source ~/.profile` (`source ~/.bashrc`)

## Setup git hook

Run `python infrastructure/tools/install_hooks.py`

This will allow to perform some code checks locally before posting changes to server.

## Install pylint - python checker

`sudo apt-get install pylint`

## Install cpplint - c++ style checker

```
sudo apt-get install python-pip
sudo pip install cpplint
```

## Build

In the project root call

`./waf configure --logger=log4cplus clean_release build_release clean_debug build_debug -v -j2`

Clang is used by defaul. Fallback to GCC if Clang not found. To use GCC call

`CXX=g++ ./waf configure`

### Build with sanitizers (clang)

Use the following option for configuration `--sanitize`. Applicable to debug build only. Known values are:

- `asan`: address
- `tsan`: thread
- `msan`: memory
- `ubsan`: undefined behaviour

Example

```
./waf configure --logger=log4cplus --sanitize=asan
./waf build_debug -v
ASAN_OPTIONS="detect_leaks=1" ./build/debug/testrunner
```

## Run

Run from project root. It's expected that config is located in the project root.

`build/debug/testrunner`

## Basic usage

### Init logging and log with global logger

```
#include "logger.h"

DECLARE_GLOBAL_GET_LOGGER("Logger.Global")

int main(int argc, char** argv) {
  INIT_LOGGER("logger.cfg");

  LOG_TRACE("Trace global line");
  LOG_DEBUG("Debug global line");
  LOG_INFO("Info global line");
  LOG_WARN("Warn global line");
  LOG_ERROR("Error global line");
  LOG_FATAL("Fatal global line");
}
```
### Class logger and traces

```
namespace Foo {
class Bar {
 public:
  DECLARE_GET_LOGGER("Foo.Bar")

  void OutputTestLogLines() {
    LOG_TRACE("Trace class line");
    LOG_DEBUG("Debug class line");
    LOG_INFO("Info class line");
    LOG_WARN("Warn class line");
    LOG_ERROR("Error class line");
    LOG_FATAL("Fatal class line");
  }

  void OutputAutoTrace() {
    LOG_AUTO_TRACE();
    LOG_TRACE("Method with auto trace");
  }
};
}  // namespace Foo
```
