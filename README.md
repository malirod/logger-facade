# boost-log
Boost logger sample

##Setup

###Install Gradle

Gradle is used to build project. This project requires java 6+.

Install Java

```
sudo add-apt-repository ppa:webupd8team/java
sudo apt-get update
sudo apt-get install oracle-java8-installer
```

Install Gradle

- download [Gradle 3.0](https://services.gradle.org/distributions/gradle-3.0-bin.zip)
- unzip to some dir, e.g. to `~/grandle-3.0`
- Add env vars to `~./profile` or `~./bashrc`

```
export GRADLE_HOME=~/gradle-3.0
PATH="$GRADLE_HOME/bin:$PATH"
```
- restart terminal, or reload config with `source ~/.profile` (`source ~/.bashrc`)

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
