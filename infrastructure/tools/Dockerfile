FROM ubuntu:16.04

ENV HOME /root
ENV PROJECT_ROOT $HOME/project
ENV BOOST_BIN $HOME/libs/boost-bin
ENV BOOST_BRANCH boost-1.61-ubuntu-16.04-x64-clang-5.8-release-c++11-static-multi
ENV BOOST_HOME $BOOST_BIN/build
ENV LOG4CPLUS_HOME $HOME/libs/log4cplus-1.2.0/build

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
RUN wget -O log4cplus-1.2.0.tar.xz http://heanet.dl.sourceforge.net/project/log4cplus/log4cplus-stable/1.2.0/log4cplus-1.2.0.tar.xz
RUN tar xf log4cplus-1.2.0.tar.xz
WORKDIR $HOME/libs/log4cplus-1.2.0
RUN ./configure CXX="clang++" --disable-shared --enable-static --prefix=$PWD/build
RUN make -j$(nproc) && make install

WORKDIR $PROJECT_ROOT
