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
git diff --check $COMMITS_RANGE --color
if [ "$?" -ne "0" ]; then
    echo -e "$TEXT_ERROR" "Your changes introduce whitespace errors" "$TEXT_DEFAULT"
    exit 1
fi
