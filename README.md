[![Build Status](https://travis-ci.org/malirod/boost-log.svg?branch=master)](https://travis-ci.org/malirod/boost-log)

# boost-log
Boost logger sample

##Platform

Ubuntu 16.04: Clang 3.8, GCC 5.4

C++11 Standard is used.

##Setup

###Install Boost

####Use pre-build Boost lib

Use the following repository: https://github.com/malirod/boost-bin

Use appropriate branch. Branch name containts info about used env and options to build Boost.

E.g.

https://github.com/malirod/boost-bin/tree/boost-1.61-ubuntu-16.04-x64-clang-5.8-release-c++11-static-multi

####Build Boost manually from sources
If pre-build version doesn't fit your needs (OS, compiler etc) then build Boost manually.

Download and unzip to some dir (e.g. `~/libs/boost_1_61_0`) latest stable version from [Boost site](http://www.boost.org/).

Go the unzipped directory and run commands

```
./bootstrap.sh --prefix=./build
./b2 --with=all -j2 install
```

####Set environment variable
Set env pointing to the boost install dir (in ~/.profile or ~/.bashrc)

`export BOOST_HOME=~/libs/boost_1_61_0/build`

Restart terminal, or reload config with `source ~/.profile` (`source ~/.bashrc`)

##Setup git hook

Run `python tools/infrastructure/install_hooks.py`

This will allow to perform some code checks locally before posting changes to server.

##Install pylint - python checker

`sudo apt-get install pylint`

##Build

In the project root call

`./waf configure clean_release build_release clean_debug build_debug -v -j2`

GCC is used by defaul. This can be changed in the folling way.

`CXX=clang++ ./waf configure`

Use '--init' flag during build to prepare for execution. E.g.

`./waf build_debug --init`

This will copy default config to the project root.

##Run

Run from project root. It's expected that config is located in the project root.

`build/debug/testrunner`
