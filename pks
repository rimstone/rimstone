#!/bin/bash
# SPDX-License-Identifier: Apache-2.0
# Copyright 2019 Gliim LLC. 
# Licensed under Apache License v2. See LICENSE file.
# On the web http://golf-lang.github.io/ - this file is part of Gliim framework.

#./pks [git-pwd]
# git-pwd is to run it non-interactively

#
#Do NOT run this script unless you are upstream developer! Internal use only.
#
#get the part that deals with signing of packages,changelog and version for the
#official Gliim release.
#

set -eE -o functrace
trap 'echo "Error: status $?, $(caller), line ${LINENO}"' ERR

if [ ! -d "$(pwd)/rv" ]; then
    rm -rf rv
    mkdir -p rv
    cd rv
    if [ "$1" != "" ]; then
        git clone https://golf:$1@bitbucket.org/golf/private_golf.git .
    else
        git clone https://golf@bitbucket.org/golf/private_golf.git .
    fi
    cd ..
fi 

rv/pks
