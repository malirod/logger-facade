#!/bin/bash

COMMITS_RANGE=$1
echo "Processing commits range: $COMMITS_RANGE"

TEXT_DEFAULT="\\033[0;39m"
TEXT_INFO="\\033[1;32m"
TEXT_ERROR="\\033[1;31m"
TEXT_UNDERLINE="\\0033[4m"
TEXT_BOLD="\\0033[1m"

##################################################################
### Check for odd whitespace
##################################################################

echo -e "$TEXT_INFO" "Checking odd whitespaces" "$TEXT_DEFAULT"
git diff --check $COMMITS_RANGE --color
if [ "$?" -ne "0" ]; then
    echo -e "$TEXT_ERROR" "Your changes introduce whitespace errors" "$TEXT_DEFAULT"
    exit 1
fi
echo -e "$TEXT_INFO" "PASSED" "$TEXT_DEFAULT"

##################################################################
### Clang-format check
##################################################################

echo -e "$TEXT_INFO" "Checking cpp style with clang-format" "$TEXT_DEFAULT"

# Auto update c++ files with clang-format. Use default clang-format.
# Use update-alternatives to set default clang.

# set path to clang-format binary
CLANG_FORMAT=$(command -v clang-format)
# Verify clang-format. Note: 3.6 and 3.7 produces different formatting for the same config
CLANG_FORMAT_REQUIRED_VERSION=3.
CLANG_FORMAT_CURRENT_VERSION=$($CLANG_FORMAT -version)

# remove any older patches from previous commits. Set to true or false.
DELETE_OLD_PATCHES=true

if [ ! -x "$CLANG_FORMAT" ] ; then
    echo -e "$TEXT_ERROR"
    echo -e "Error: clang-format executable not found."
    echo -e "Set the correct path in $(canonicalize_filename "$0")."
    echo -e "$TEXT_DEFAULT"
    exit 1
fi

if [[ $CLANG_FORMAT_CURRENT_VERSION != *$CLANG_FORMAT_REQUIRED_VERSION* ]]
then
  echo -e "$TEXT_ERROR" "Wrong version of the clang-format. Required: $CLANG_FORMAT_REQUIRED_VERSION. Got: $CLANG_FORMAT_CURRENT_VERSION" "$TEXT_DEFAULT"
  exit 1
fi

# create a random filename to store our generated patch
prefix="pre-commit-clang-format"
suffix="$(date +%s)"
patch="/tmp/$prefix-$suffix.patch"

# clean up any older clang-format patches
$DELETE_OLD_PATCHES && rm -f /tmp/$prefix*.patch

CPP_FILES=$(git diff $COMMITS_RANGE --name-only --diff-filter=ACM | grep -e "\.cc$" -e "\.cpp$" -e "\.c$" -e "\.h$" -e "\.hpp$")

# create one patch containing all changes to the files
# git diff-index --cached --diff-filter=ACMR --name-only $against -- | while read file;
for cpp_file in $CPP_FILES
do
    # clang-format our sourcefile, create a patch with diff and append it to our $patch
    # The sed call is necessary to transform the patch from
    #    --- $file timestamp
    #    +++ - timestamp
    # to both lines working on the same file and having a a/ and b/ prefix.
    # Else it can not be applied with 'git apply'.
    "$CLANG_FORMAT" -style=file "$cpp_file" | \
        diff -u "$cpp_file" - | \
        sed -e "1s|--- |--- a/|" -e "2s|+++ -|+++ b/$cpp_file|" >> "$patch"
done

# if no patch has been generated all is ok, clean up the file stub and exit
if [ ! -s "$patch" ] ; then
    rm -f "$patch"
else
    # a patch has been created, notify the user and exit
    echo -e "$TEXT_ERROR"
    echo -e "The following differences were found between the code to commit "
    echo -e "and the clang-format rules:"
    echo -e "$TEXT_INFO"
    cat $patch
    echo -e "$TEXT_ERROR"
    echo -e "You can apply these changes with:"
    echo -e "git apply $patch"
    echo -e "(may need to be called from the root directory of your repository)"
    echo -e "Aborting commit. Apply changes and commit again or skip checking with"
    echo -e " --no-verify (not recommended)."
    echo -e "$TEXT_DEFAULT"

    exit 2
fi

echo -e "$TEXT_INFO" "PASSED" "$TEXT_DEFAULT"

##################################################################
### Auto-check python code with pylint
##################################################################

echo -e "$TEXT_INFO" "Checking python style with pylint" "$TEXT_DEFAULT"

# wscript is a python script
PYTHON_FILES=$(git diff $COMMITS_RANGE --name-only --diff-filter=ACM | grep -e "\.py$" -e "wscript")

if [ -n "$PYTHON_FILES" ]; then
    pylint --rcfile=.pylintrc $PYTHON_FILES
    if [ "$?" -ne "0" ]; then
        echo -e "$TEXT_ERROR" "Pylint reports about the issues in the python scripts" "$TEXT_DEFAULT"
        exit 3
    fi
fi

echo -e "$TEXT_INFO" "PASSED" "$TEXT_DEFAULT"

exit 0
