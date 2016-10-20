FROM ubuntu:16.04

ENV HOME /root
ENV PROJECT_ROOT $HOME/project
ENV BOOST_BIN $HOME/libs/boost-bin
ENV BOOST_BRANCH boost-1.61-ubuntu-16.04-x64-clang-5.8-release-c++11-static-multi
ENV BOOST_HOME $BOOST_BIN/build
ENV LOG4CPLUS_HOME $HOME/libs/log4cplus/build

ADD . $PROJECT_ROOT

WORKDIR $HOME

RUN apt-get update
RUN apt-get install -y git
RUN apt-get install -y clang clang-format g++
RUN apt-get install -y pylint
RUN apt-get install -y python-pip
RUN pip install cpplint

RUN git clone https://github.com/malirod/boost-bin.git $BOOST_BIN
WORKDIR $BOOST_BIN
RUN git fetch
RUN git checkout ${BOOST_BRANCH}
RUN tar -xzf build.tar.gz

WORKDIR $HOME/libs
RUN apt-get install -y autotools-dev automake wget
RUN apt-get install autotools-dev automake
RUN git clone https://github.com/log4cplus/log4cplus.git
WORKDIR $HOME/libs/log4cplus
# Lets stick to some specific commit
# Use master branch since it has no auto_ptr and it's buildable with gcc 5.4
RUN git checkout -b fixed-version 953d843127a3c79ca3d020bb334c85d36ce0ae57
RUN git submodule update --init --recursive
RUN ./scripts/fix-timestamps.sh
RUN ./configure CXX="clang++" --disable-shared --enable-static --prefix=$PWD/build
RUN make -j$(nproc) && make install

WORKDIR $PROJECT_ROOT
