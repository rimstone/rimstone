#!/bin/bash
#SPDX-License-Identifier: Apache-2.0
#Copyright 2018-2025 Gliim LLC.  
#Licensed under Apache License v2. See LICENSE file.
#On the web http://golf-lang.com/ - this file is part of Golf framework.

#Setup Golf environment for bash shell

export GG_ROOT="$HOME/.golf" 
export PATH="$GG_ROOT/bin":"$GG_ROOT/man/man2gg/":$PATH 
export C_INCLUDE_PATH="$GG_ROOT/include":$C_INCLUDE_PATH 
export MANPATH="$GG_ROOT/man/man2gg/":$MANPATH
