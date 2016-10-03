import os

CPP_FLAGS = [
    '-Wextra',
    '-DDEBUG',
    '-std=c++11',
    # ...and the same thing goes for the magic -x option which specifies the
    # language that the files to be compiled are written in. This is mostly
    # relevant for c++ headers.
    # For a C project, you would set this to 'c' instead of 'c++'.
    '-x',
    'c++',
    '-I/usr/local/include',
    '-Isrc',
    '-Itest',
    '-Ithirdparty/gtest',
    '-Ithirdparty',
    '-I.',
    '-I${BOOST_HOME}/include']


SOURCE_EXTENSIONS = ['.cpp', '.cxx', '.cc', '.c', '.m', '.mm']


def directory_of_this_script():
    return os.path.dirname(os.path.abspath(__file__))


def expand_rel_path(flags, working_directory):
    if not working_directory:
        return list(flags)
    new_flags = []
    make_next_absolute = False
    path_flags = ['-isystem', '-I', '-iquote', '--sysroot=']
    for flag in flags:
        new_flag = flag

        if make_next_absolute:
            make_next_absolute = False
            if not flag.startswith('/'):
                new_flag = os.path.join(working_directory, flag)

        for path_flag in path_flags:
            if flag == path_flag:
                make_next_absolute = True
                break

            if flag.startswith(path_flag):
                path = flag[len(path_flag):]

                # expand env vars if present
                if '$' in path:
                    new_flag = path_flag + os.path.expandvars(path)
                else:
                    new_flag = path_flag + os.path.join(working_directory, path)
                break

        if new_flag:
            new_flags.append(new_flag)
    return new_flags


def is_header_file(filename):
    extension = os.path.splitext(filename)[1]
    return extension in ['.h', '.hxx', '.hpp', '.hh']


def FlagsForFile(filename, **kwargs): # pylint: disable=invalid-name
    ''' This method is called by YCM to get compilation flags for each file '''
    _ = filename, kwargs
    relative_to = directory_of_this_script()
    final_flags = expand_rel_path(CPP_FLAGS, relative_to)

    return {'flags': final_flags, 'do_cache': True}

# "Test flags"
# print flags_for_file("test.cpp")
