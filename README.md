# boost-log
Boost logger sample

##Platform

Ubuntu 16.04: Clang 3.8, GCC 5.4

C++11 Standard is used.

##Setup

###Install Boost

- download and unzip to some dir (e.g. `~/libs/boost_1_61_0`) latest stable version from [Boost site](http://www.boost.org/).
- go the unzipped directory and run commands

```
./bootstrap.sh --prefix=./build
./b2 --with=all -j2 install
```
- set env pointing to the boost install dir (in ~/.profile or ~/.bashrc)

`export BOOST_HOME=~/libs/boost_1_61_0/build`

- restart terminal, or reload config with `source ~/.profile` (`source ~/.bashrc`)

##Build

In the project root call

`./waf configure clean_release build_release clean_debug build_debug -v -j2`

GCC is used by defaul. This can be changed in the folling way.

`CXX=clang++ ./waf configure`
