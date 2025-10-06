#!/bin/bash
#SPDX-License-Identifier: Apache-2.0
#Copyright 2018-2025 Gliim LLC.
#Licensed under Apache License v2. See LICENSE file.
#On the web http://rimstone-lang.com/ - this file is part of RimStone framework.

#Install script for RimStone. Parameter is "web", "package" or "source"
#"web" downloads source from the web (the default),
#"package" assumes there's a system rimstone package installed with source in system dir
#"source" assumes you've download rimstone either yours or cloned via git and you're running this from that directory

#display error context if rim.has a shell error, source file bash only
set -eE -o functrace
trap 'echo "Error: status $?, $(caller), line ${BASH_SOURCE[0]}/${LINENO}"' ERR

#
PAR="$1"
#for scripting support, install all toolkit packages without a prompt, suffix with -def for default.
DEF=0
if [[ "$PAR" =~ "all" ]]; then 
    DEF=1
fi
#

#cannot run as run as it might mess up permissions.
if [[ $EUID -eq 0 ]]; then error "You cannot run this script as root or sudo";  fi
#user must have home directory
if [ ! -d "$HOME"  ]; then error "This user has no home directory or it is not accessible"; fi

if [ ! -f "./rimlib" ]; then echo "Cannot find RimStone source code in current directory." ;return 1; fi

#get environment and functions used below
. ./rimlib ""

NOINST="Cannot install package, likely because your package manager is unknown to RimStone."

#install packages: $1 is a for toolkit or b for base packages
function install_pkgs(){
    E=0; PMISS=$(./rimlib -$1) || E=$?
    if [ "$E" != "0" ]; then
        if [ "$DEF" == "0" ]; then
            SHOW_INST=$(exec_install "$PMISS" 0)
            if [ "$1" == "a" ]; then
                read -p $'*** Some toolkit packages are not installed. Here is how to install them:\n\n'"$SHOW_INST"$'\n\nIf you will use features provided by these toolkits, you will need to install them. You can also install them later; RimStone will let you know and show the installation instructions when you compile a program that needs such a feature. If you are not sure, or do not want interruptions later, you can install the toolkit packages now.\nDo you want to install them now (you will need sudo privilege)? (y/N)' inst;
            else
                read -p $'*** Some required base packages are not installed. Here is how to install them:\n\n'"$SHOW_INST"$'\n\nDo you want to install them now (you will need sudo privilege)? (y/N)' inst; 
            fi
        else
            inst="y"
        fi
        if [[ "$inst" == "y" || "$inst" == "Y" || "$inst" == "yes" || "$inst" == "Yes" || "$inst" == "YES" ]]; then
            exec_install "$PMISS" 1
            E=0; PMISS=$(./rimlib -$1) || E=$?
            if [ "$E" != "0" ]; then  return 1; fi
        fi
    fi
}

#in case base packages weren't found, run discovery again to find toolkit packages
discovery

#install base
E=0; install_pkgs "b" || E=$?
if [ "$E" != "0" ]; then
    echo "Installation of required packages failed. Please install packages above and try the installation again."; exit 1;
fi
#install tookit. not installing toolkit is not an error. RimStone developer will be presented with a way to install them
#when (and if) they are needed.
install_pkgs "a" || true


#make .rimstone and binaries, create directory structure
echo "Making RimStone binaries and libraries, please wait..."
make install

if [[ -f /etc/selinux/config && -f "/usr/share/selinux/devel/Makefile" ]]; then
    #this executes always because it's impossible to say if the policy is the same, even if they are the same as before
    RRLIB=$HOME/.rimstone/lib
    RRSEL="sudo $RRLIB/selinux/selinux.setup"
    if [ "$DEF" == "0" ]; then
        read -p "Since you have SELinux enabled, you must setup SELinux for RimStone. sudo privilege is required by the Operating System to complete this. The script [$RRSEL] will run next. Press Enter to continue."
    fi
    eval $RRSEL || true
fi


echo "Setting environment variables..."
#remove old setup
sed -i '/#rimstone-setup\s*$/d' $HOME/.bashrc
#remove old product name
sed -i '/#golf-setup\s*$/d' $HOME/.bashrc
#setup RimStone
#check if file exists if RimStone is not there
echo "if [ -f $HOME/.rimstone/lib/rimsetenv.sh ]; then . $HOME/.rimstone/lib/rimsetenv.sh; fi #rimstone-setup" >>$HOME/.bashrc
#execute for this session in case it's sourced
. $HOME/.rimstone/lib/rimsetenv.sh
#setup vim highlighting
gg -m

#setup minimum permissions (inlcuding for Unix sockets)
echo "Setting permissions..."
chmod 0711 $HOME

E=0; which mandb 2>/dev/null || E=$?
if [ "$E" == "0" ]; then
    echo "Setting man pages..."
    ECODE=0
    mandb -u -c || ECODE=$? >/dev/null
else
    echo "man not available, use 'gg --man all|topic' to get help from command line"
fi

