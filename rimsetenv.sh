#!/bin/bash
#SPDX-License-Identifier: Apache-2.0
#Copyright 2018-2025 Gliim LLC.  
#Licensed under Apache License v2. See LICENSE file.
#On the web http://rimstone-lang.com/ - this file is part of RimStone framework.

#Setup RimStone environment for bash shell

export RIM_ROOT="$HOME/.rimstone" 
export PATH="$RIM_ROOT/bin":"$RIM_ROOT/man/man2/":$PATH 
export C_INCLUDE_PATH="$RIM_ROOT/include":$C_INCLUDE_PATH 
export MANPATH="$RIM_ROOT/man/man2/":$MANPATH
