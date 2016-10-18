FROM ubuntu:16.04

ENV HOME /root
ENV PROJECT_ROOT $HOME/project
ENV BOOST_BIN $HOME/libs/boost-bin
ENV BOOST_BRANCH boost-1.61-ubuntu-16.04-x64-clang-5.8-release-c++11-static-multi
ENV BOOST_HOME $BOOST_BIN/build

ADD . $PROJECT_ROOT

WORKDIR $HOME

RUN apt-get update
RUN apt-get install -y git
RUN apt-get install -y clang clang-format
RUN apt-get install -y pylint

RUN git clone https://github.com/malirod/boost-bin.git $BOOST_BIN
WORKDIR $BOOST_BIN
RUN git fetch
RUN git checkout ${BOOST_BRANCH}
RUN tar -xzf build.tar.gz

WORKDIR $PROJECT_ROOT
