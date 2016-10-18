# -*- coding: utf-8 -*-

import os
import shutil
from multiprocessing import cpu_count
from waflib.Build import (BuildContext, CleanContext, # pylint: disable=import-error
                          InstallContext, UninstallContext)

top = '.' # pylint: disable=invalid-name
out = 'build' # pylint: disable=invalid-name


def options(opt):
    opt.load('compiler_cxx')
    opt.add_option('--init', action='store_true', default=False,
                   help='Init output dir after build')


def configure(conf):
    print 'Configuring the project in ' + conf.path.abspath()

    boost_include_path = os.path.join(os.getenv('BOOST_HOME'), "include")
    boost_libs_path = os.path.join(os.getenv('BOOST_HOME'), "lib")

    def common_setup(env):
        env.CXXFLAGS += ['-std=c++11', '-Wextra', '-Werror', '-Wpedantic']
        env.LIBPATH += [boost_libs_path]
        env.INCLUDES += [boost_include_path]
        env.LIB += ['pthread']
        env.STLIB += ['boost_log_setup', 'boost_log', 'boost_thread',
                      'boost_system', 'boost_filesystem', 'boost_regex']

    # Setup debug configuration
    conf.setenv('debug')
    conf.load('compiler_cxx')
    common_setup(conf.env)
    conf.env.CXXFLAGS += ['-g']
    conf.env.CXXFLAGS += ['-O0']

    # Setup release configuration
    conf.setenv('release')
    conf.load('compiler_cxx')
    common_setup(conf.env)
    conf.env.CXXFLAGS += ['-O3']


def post_build(bld):
    if bld.options.init:
        print 'Post build step: copy logging config to the project root'
        source_cfg = os.path.join(
            bld.path.abspath(), 'infrastructure', 'config', 'logger.cfg')
        shutil.copy(source_cfg, bld.path.abspath())


def build(bld):
    if not bld.variant:
        bld.fatal('call "waf build_debug" or "waf build_release",'
                  ' and try "waf --help"')

    bld.jobs = cpu_count()
    bld.add_post_fun(post_build)

    bld_common_includes = ['src', 'thirdparty/gtest']

    special_files = ['src/logger.cc']
    special_target = 'special_objects'

    bld.objects(source=special_files[0],
                target=special_target,
                cxxflags=['-Wno-unused-parameter'],
                includes=bld_common_includes)

    bld.program(source=bld.path.ant_glob(['**/*.cc'], excl=special_files),
                target='testrunner',
                includes=bld_common_includes,
                use=special_target)


for x in 'debug release'.split():
    for y in (BuildContext, CleanContext, InstallContext, UninstallContext):
        name = y.__name__.replace('Context', '').lower()

        class Tmp(y): # pylint: disable=too-few-public-methods
            cmd = name + '_' + x
            variant = x
