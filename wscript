#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os

top = '.'
out = 'build'


def options(opt):
    opt.load('compiler_cxx')


def configure(conf):
    print ('Configuring the project in ' + conf.path.abspath())

    boost_include_path = os.path.join(os.getenv('BOOST_HOME'), "include")
    boost_libs_path = os.path.join(os.getenv('BOOST_HOME'), "lib")

    conf.load('compiler_cxx')

    conf.env.LIBPATH += [boost_libs_path]
    conf.env.INCLUDES += [boost_include_path]
    conf.env.LIB += ['pthread']
    conf.env.STLIB += ['boost_log', 'boost_thread', 'boost_system']


def build(bld):
    bld.program(source=bld.path.ant_glob('**/*.cc'),
                target='testrunner',
                includes=['src', 'thirdparty/gtest'])
