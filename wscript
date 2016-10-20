# -*- coding: utf-8 -*-

import os
import shutil
from multiprocessing import cpu_count
from waflib.Build import (BuildContext, CleanContext, # pylint: disable=import-error
                          InstallContext, UninstallContext)
from waflib.Tools.compiler_cxx import cxx_compiler # pylint: disable=import-error

top = '.' # pylint: disable=invalid-name
out = 'build' # pylint: disable=invalid-name


def options(opt):
    opt.load('compiler_cxx')
    opt.add_option('--init', action='store_true',
                   default=None,
                   help='Init output dir after build')
    opt.add_option('--sanitize',
                   action='store',
                   default=None,
                   help='Build with sanitizer. (asan, tsan, msan, ubsan)',
                   choices=('asan', 'tsan', 'msan', 'ubsan'))
    opt.add_option('--logger', action='store', default='none',
                   help='Set logger to use. (none, boost, log4cplus) [default: none]',
                   choices=('none', 'boost', 'log4cplus'))


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


def setup_common(ctx):
    ctx.env.CXXFLAGS += ['-std=c++11', '-Wextra', '-Werror', '-Wpedantic']
    ctx.env.LIB += ['pthread']

    if ctx.options.logger == 'none':
        ctx.env.CXXFLAGS += ['-DDISABLE_LOGGER']
    elif ctx.options.logger == 'boost':
        if os.getenv('BOOST_HOME') is None:
            ctx.fatal('BOOST_HOME is not set.')
        boost_include_path = os.path.join(os.getenv('BOOST_HOME'), "include")
        boost_libs_path = os.path.join(os.getenv('BOOST_HOME'), "lib")
        ctx.env.LIBPATH += [boost_libs_path]
        ctx.env.INCLUDES += [boost_include_path]
        ctx.env.STLIB += ['boost_log_setup', 'boost_log', 'boost_thread',
                          'boost_system', 'boost_filesystem', 'boost_regex']
        ctx.env.CXXFLAGS += ['-DUSE_BOOST_LOGGER']
    elif ctx.options.logger == 'log4cplus':
        if os.getenv('LOG4CPLUS_HOME') is None:
            ctx.fatal('LOG4CPLUS_HOME is not set.')
        ctx.env.CXXFLAGS += ['-DUSE_LOG4CPLUS_LOGGER']
        log4cplus_include_path = os.path.join(os.getenv('LOG4CPLUS_HOME'), "include")
        log4cplus_libs_path = os.path.join(os.getenv('LOG4CPLUS_HOME'), "lib")
        ctx.env.LIBPATH += [log4cplus_libs_path]
        ctx.env.INCLUDES += [log4cplus_include_path]
        ctx.env.STLIB += ['log4cplus']


def configure(ctx):
    print 'Configuring the project in ' + ctx.path.abspath()

    # Setup debug configuration
    ctx.setenv('debug')
    ctx.load('compiler_cxx')
    setup_common(ctx)
    ctx.env.CXXFLAGS += ['-g']
    ctx.env.CXXFLAGS += ['-O0']
    # Add sanitizers if required
    if ctx.options.sanitize:
        setup_sanitizer(ctx)

    # Setup release configuration
    ctx.setenv('release')
    ctx.load('compiler_cxx')
    setup_common(ctx)
    ctx.env.CXXFLAGS += ['-O3']

    if ctx.options.init:
        if ctx.options.logger != 'none':
            src_cfg = os.path.join(
                ctx.path.abspath(), 'infrastructure', 'config', 'logger.cfg')
            shutil.copyfile('{}.{}'.format(src_cfg, ctx.options.logger),
                            os.path.join(ctx.path.abspath(), 'logger.cfg'))


def post_build(dummy_ctx):
    pass


def build(ctx):
    if not ctx.variant:
        ctx.fatal('call "waf build_debug" or "waf build_release",'
                  ' and try "waf --help"')

    ctx.jobs = cpu_count()
    ctx.add_post_fun(post_build)

    bld_common_includes = ['src', 'thirdparty/gtest']

    special_files = ['src/util/logger.cc']
    special_target = 'special_objects'

    ctx.objects(source=special_files[0],
                target=special_target,
                cxxflags=['-Wno-unused-parameter'],
                includes=bld_common_includes)

    ctx.program(source=ctx.path.ant_glob(['**/*.cc'], excl=special_files),
                target='testrunner',
                includes=bld_common_includes,
                use=special_target)


for x in 'debug release'.split():
    for y in (BuildContext, CleanContext, InstallContext, UninstallContext):
        name = y.__name__.replace('Context', '').lower()

        class Tmp(y): # pylint: disable=too-few-public-methods
            cmd = name + '_' + x
            variant = x

# Prefer clang compiler over gcc
cxx_compiler['linux'] = ['clang++', 'g++', 'icpc']
