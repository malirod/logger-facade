#!/usr/bin/env python2
# -*- coding: utf-8 -*-

""" Misc utils used by other modules """

import os
import contextlib


def setup_working_dir():
    """ Prepare current working dir (in case if script is called from
    outside). Set it to the project root.
    """
    os.chdir(os.path.join(os.path.dirname(
        os.path.realpath(__file__)), os.pardir, os.pardir))


def run_cmd(cmd_line):
    """ Run command in system shell """
    import subprocess

    return subprocess.call(cmd_line, shell=True)


def walk_dir(directory, action, recursive=True):
    """ Walks a directory, and executes the action on each file """
    directory = os.path.abspath(directory)
    for item in [item for item in os.listdir(directory)
                 if item not in [os.curdir, os.pardir]]:
        file_path = os.path.join(directory, item)
        action(file_path)
        if recursive and os.path.isdir(file_path):
            walk_dir(file_path, action)


@contextlib.contextmanager
def working_directory(path):
    """ A context manager which changes the working directory to the given
    path, and then changes it back to it's previous value on exit.
    """
    prev_cwd = os.getcwd()
    os.chdir(path)
    try:
        yield
    finally:
        os.chdir(prev_cwd)
