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
    opt.add_option('--sanitize', action='store', default=False,
                   help='Build with sanitizer: [asan, tsan, msan, ubsan]')

def setup_sanitizer(ctx):
    if ctx.options.sanitize == 'asan':
        ctx.env.CXXFLAGS += ['-fsanitize=address']
        ctx.env.LINKFLAGS += ['-fsanitize=address']
    elif ctx.options.sanitize == 'tsan':
        ctx.env.CXXFLAGS += ['-fsanitize=thread']
        ctx.env.LINKFLAGS += ['-fsanitize=thread']
    elif ctx.options.sanitize == 'msan':
        ctx.env.CXXFLAGS += ['-fsanitize=memory']
        ctx.env.LINKFLAGS += ['-fsanitize=memory']
    elif ctx.options.sanitize == 'ubsan':
        ctx.env.CXXFLAGS += ['-fsanitize=undefined']
        ctx.env.LINKFLAGS += ['-fsanitize=undefined']
    else:
        print 'Invalid value for "sanitize" option. Expected: [asan, tsan, msan, ubsan]'


def setup_common(ctx):
    boost_include_path = os.path.join(os.getenv('BOOST_HOME'), "include")
    boost_libs_path = os.path.join(os.getenv('BOOST_HOME'), "lib")

    ctx.env.CXXFLAGS += ['-std=c++11', '-Wextra', '-Werror', '-Wpedantic']
    ctx.env.LIBPATH += [boost_libs_path]
    ctx.env.INCLUDES += [boost_include_path]
    ctx.env.LIB += ['pthread']
    ctx.env.STLIB += ['boost_log_setup', 'boost_log', 'boost_thread',
                      'boost_system', 'boost_filesystem', 'boost_regex']


def configure(conf):
    print 'Configuring the project in ' + conf.path.abspath()

    # Setup debug configuration
    conf.setenv('debug')
    conf.load('compiler_cxx')
    setup_common(conf)
    conf.env.CXXFLAGS += ['-g']
    conf.env.CXXFLAGS += ['-O0']
    # Add sanitizers if required
    if conf.options.sanitize:
        setup_sanitizer(conf)

    # Setup release configuration
    conf.setenv('release')
    conf.load('compiler_cxx')
    setup_common(conf)
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

    special_files = ['src/util/logger.cc']
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
