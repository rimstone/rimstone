#SPDX-License-Identifier: Apache-2.0
#Copyright 2018-2025 Gliim LLC.  
#Licensed under Apache License v2. See LICENSE file.
#On the web http://rimstone-lang.com/ - this file is part of RimStone framework.

#makefile for RimStone. This file builds and installs RimStone. sudo make install does it all, though you can do make first. make clean cleans it out.

.ONESHELL:
SHELL:=/bin/bash


#this works even if sudo'd (unlike $USER)
RRUSER=$(shell whoami) 
export RRUSER

#
#check DESTDIR, if not specified, default is $HOME/.rimstone
ifeq ("$(HOME)",)
    HOME=$(shell eval echo ~$(RRUSER))
endif
ifeq ($(strip $(DESTDIR)),)
	DESTDIR:=$(HOME)/.rimstone
    RES=$(shell install -m 0711 -d $(DESTDIR))
endif
#For socket access, minimum of 711 home directory is required
RES=$(shell chmod 711 $(HOME))
#

#Set make flags only at top level, and then use $(MAKE) to inherit them
ifeq ($(MAKELEVEL),0)
    #use all CPUs if not specified
    MJOBS=$(shell nproc) 
    #check if -j or --jobs is there, if not add it
    ifndef MAKEFLAGS
        MAKEFLAGS:=-j$(MJOBS)
    else ifeq ($(findstring --jobs,$(MAKEFLAGS)),) 
        ifeq ($(findstring -j,$(MAKEFLAGS)),) 
            MAKEFLAGS:=$(MAKEFLAGS) -j$(MJOBS)
        endif
    endif
    ifeq ($(findstring --no-print-directory,$(MAKEFLAGS)),) 
        MAKEFLAGS += --no-print-directory
    endif
endif

#verbose, show all
ifeq ($(V),1)
    AT:=
else
    AT:=@
endif

#include DEBUG flag, which includes code that normally never executes, even in Devel; this is for RimStone developers only
ifeq ($(D),1)
    DBG:=-DDEBUG -g3 -O
else
    DBG:=
endif

#make sure release and devel dirs are there, they are always needed
RELDIR:=$(shell mkdir -p release)
RELDIRBT:=$(shell mkdir -p release/btrace)
RELDIRFCGI:=$(shell mkdir -p release/fcgi)
DEVDIR:=$(shell mkdir -p devel)
DEVDIRBT:=$(shell mkdir -p devel/btrace)
DEVDIRFCGI:=$(shell mkdir -p devel/fcgi)

.DEFAULT_GOAL=build_spec_all

#profiler
PROF=:$(P)

#short cut for Address Sanitizer, internal only
ASAN:=$(A)
#ASAN instrumentation flag
ifeq ($(ASAN),1)
    ASAN=-fsanitize=address -fsanitize-recover=address
    ASAN_OPTIONS=detect_leaks=0:log_path=asan:halt_on_error=0
    export ASAN_OPTIONS
else
    ASAN=
endif

#flags for release vs devel. if D=1 is used, then release is also devel with debug add-ons
ifeq ($(strip $(REL)),1)
    CFLAGS_WARN_ERROR=
    OPTIM_COMP=-g -O3 $(DBG)
    OPTIM_LINK=
    LTO=-flto=auto 
    RDEST=release
    RIM_ALWAYS_INLINE=__attribute__((always_inline))
    ASAN=
    DEVEL=
else
    CFLAGS_WARN_ERROR=-Werror 
    OPTIM_COMP=-g3 $(DBG) -rdynamic
    OPTIM_LINK=-rdynamic
    LTO=
    RDEST=devel
    RIM_ALWAYS_INLINE=
    DEVEL=-DRIM_DEVEL
endif

#determine which libraries are present and which are not (0 means present)
_dummy:=$(shell . ./rimlib && \
    echo "" > .rimlib.mk && \
    echo "RIM_MARIADB_EX:=$$RIM_MARIADB_EX" >> .rimlib.mk && \
    echo "RIM_SQLITE_EX:=$$RIM_SQLITE_EX" >> .rimlib.mk && \
    echo "RIM_CURL_EX:=$$RIM_CURL_EX" >> .rimlib.mk && \
    echo "RIM_PCRE2_EX:=$$RIM_PCRE2_EX" >> .rimlib.mk && \
    echo "RIM_CRYPTO_EX:=$$RIM_CRYPTO_EX" >> .rimlib.mk && \
    echo "RIM_POSTGRES_EX:=$$RIM_POSTGRES_EX" >> .rimlib.mk && \
    echo "RIM_XML_EX:=$$RIM_XML_EX" >> .rimlib.mk && \
    echo "RIM_MARIADB_INC:=$$RIM_MARIADB_INC" >> .rimlib.mk && \
    echo "RIM_POSTGRES_INC:=$$RIM_POSTGRES_INC" >> .rimlib.mk )
include .rimlib.mk


#get pcre2 version and libs
ifeq ($(RIM_PCRE2_EX),0)
    PCRE2_VER:=$(shell pcre2-config --version)
endif
#get flags whether to include 3rd libs .h files (based on whether they exist or not)
RRINCF=
ifeq ($(RIM_MARIADB_EX),0)
    RRINCF+= -DRIM_INC_MARIADB=1
endif
ifeq ($(RIM_POSTGRES_EX),0)
    RRINCF+= -DRIM_INC_POSTGRES=1
endif
ifeq ($(RIM_SQLITE_EX),0)
    RRINCF+= -DRIM_INC_SQLITE=1
endif
ifeq ($(RIM_CURL_EX),0)
    RRINCF+= -DRIM_INC_CURL=1
endif
ifeq ($(RIM_CRYPTO_EX),0)
    RRINCF+= -DRIM_INC_CRYPTO=1
endif
ifeq ($(RIM_XML_EX),0)
    RRINCF+= -DRIM_INC_XML=1
endif
ifeq ($(RIM_PCRE2_EX),0)
    RRINCF+= -DRIM_INC_PCRE2=1
endif
#these must be the same (RIM_PLATFORM_ID,RIM_PLATFORM_VERSION) used in sys
OSNAME:=$(shell . ./sys; echo -n $${RIM_PLATFORM_ID})
OSVERSION:=$(shell . ./sys; echo -n $${RIM_PLATFORM_VERSION})

#current date for man pages
DATE:=$(shell date +"%b-%d-%Y")

#these 2 must match application name from config file from each application

CC:=gcc
CCAR:=gcc-ar rcs

#get build version and release, if .verstion not present, then empty
PACKAGE_VERSION:=$(shell . .version || true; echo $${PACKAGE_VERSION})
ifeq ($(strip $(PACKAGE_VERSION)),)
    PACKAGE_VERSION:=2
endif


#Basic paths for install, SELinux is a system path as required
V_APPS:=$(DESTDIR)/apps
V_LIB:=$(DESTDIR)/lib
V_INC:=$(DESTDIR)/include
V_BIN:=$(DESTDIR)/bin
V_TMP:=$(DESTDIR)/tmp
V_MAN:=$(DESTDIR)/man/man2
V_SHARE:=$(DESTDIR)/rimstone
V_RIM_SELINUXDIR:=/usr/share
V_RIM_ABOUT:=$(DESTDIR)/about


#XML flags
RIM_LIBXML2_INCLUDE:=$(shell pkgconf --silence-errors --cflags libxml-2.0)

#profiler flag
ifeq ($(PROF),1)
    PROF=-pg
else
    PROF=
endif


#Note that package tools (arch, debian, fedora) separate debug info and strip executables and shared libraries.
#The default -g used for non-devel package creates normal debug info. It's -g3 that creates a lot, but that's only for devel build.

#C flags are as strict as we can do, in order to discover as many bugs as early on
#Release doesn't use -Werror because with many flavors of gcc out there, chances of failing are high (dev only)
#unwind tables are mandatory on all x86_64 ABIs, so the inclusion below is really for aarch64
#we strictly use UTF-8 only (big mess otherwise)
CFLAGS=$(CFLAGS_WARN_ERROR) -Wall -Wextra -Wuninitialized -Wmissing-declarations -Wformat -Werror=format-security -Wno-format-zero-length -funsigned-char -fpic -fno-semantic-interposition  $(RIM_MARIADB_INC) $(RIM_POSTGRES_INC) $(RIM_LIBXML2_INCLUDE) -DRIM_OSNAME="\"$(OSNAME)\"" -DRIM_OSVERSION="\"$(OSVERSION)\"" -DRIM_PKGVERSION="\"$(PACKAGE_VERSION)\"" -DRIM_ALWAYS_INLINE='$(RIM_ALWAYS_INLINE)' $(OPTIM_COMP) $(ASAN) $(PROF) -fmax-errors=5 -Wdate-time -fno-stack-protector -fno-stack-clash-protection $(LTO) -funwind-tables -finput-charset=UTF-8 -fexec-charset=UTF-8 $(RRINCF) $(DEVEL)

#linker flags. 
LFLAGS_COMMON=-Wl,-z,relro
LFLAGS=-Wl,-rpath=$(V_LIB)/$(RDEST) -Wl,--enable-new-dtags -L$(V_LIB)/$(RDEST) $(OPTIM_LINK) $(LFLAGS_COMMON) -lm $(ASAN) $(PROF) $(LTO)

nothing: 
	$(AT)echo "RimStone $(PACKAGE_VERSION) development build complete."

#Copy binaries from building area to their final place. $(RDEST) is the directory (release or devel)
#no -D in install, we want it to fail if no directories; use make install for that first.
#build_spec builds into local release/devel dir; build_structure builds install folders; this copies former into the latter.
#The order doesn't matter since the two are independent folders.
copy_binaries: build_spec  build_structure
	$(AT)$(info Copying RimStone binaries ($(RDEST)))
	$(AT)install -m 0700 $(RDEST)/librimfcgi.a -t $(V_LIB)/$(RDEST)/
	install -m 0700 $(RDEST)/librimbtrace.a -t $(V_LIB)/$(RDEST)/
	if [ "$(RIM_POSTGRES_EX)" == "0" ]; then install -m 0700 $(RDEST)/librimpg.a -t $(V_LIB)/$(RDEST)/ ; fi
	install -m 0700 $(RDEST)/librimdb.a -t $(V_LIB)/$(RDEST)/
	if [ "$(RIM_SQLITE_EX)" == "0" ]; then install -m 0700 $(RDEST)/librimlite.a -t $(V_LIB)/$(RDEST)/ ; fi
	if [ "$(RIM_MARIADB_EX)" == "0" ]; then install -m 0700 $(RDEST)/librimmys.a -t $(V_LIB)/$(RDEST)/ ; fi
	if [ "$(RIM_CRYPTO_EX)" == "0" ]; then install -m 0700 $(RDEST)/librimcrypto.a -t $(V_LIB)/$(RDEST)/ ; fi
	install -m 0700 $(RDEST)/librimtree.a -t $(V_LIB)/$(RDEST)/
	if [ "$(RIM_CURL_EX)" == "0" ]; then install -m 0700 $(RDEST)/librimcurl.a -t $(V_LIB)/$(RDEST)/ ; fi
	if [ "$(RIM_XML_EX)" == "0" ]; then install -m 0700 $(RDEST)/librimxml.a -t $(V_LIB)/$(RDEST)/ ; fi
	install -m 0700 $(RDEST)/librimarr.a -t $(V_LIB)/$(RDEST)/
	install -m 0700 $(RDEST)/librimarrsort.a -t $(V_LIB)/$(RDEST)/
	if [ "$(RIM_PCRE2_EX)" == "0" ]; then install -m 0700 $(RDEST)/librimpcre2.a -t $(V_LIB)/$(RDEST)/ ; fi
	install -m 0700 $(RDEST)/libsrvcrim.a -t $(V_LIB)/$(RDEST)/
	install -m 0700 $(RDEST)/librim.a -t $(V_LIB)/$(RDEST)/
	install -m 0700 $(RDEST)/librimcli.a -t $(V_LIB)/$(RDEST)/
	install -m 0700 $(RDEST)/librimscli.a -t $(V_LIB)/$(RDEST)/
	install -m 0600 $(RDEST)/stub_sqlite.o -t $(V_LIB)/$(RDEST)/
	install -m 0600 $(RDEST)/stub_postgres.o -t $(V_LIB)/$(RDEST)/
	install -m 0600 $(RDEST)/stub_mariadb.o -t $(V_LIB)/$(RDEST)/
	install -m 0600 $(RDEST)/stub_gendb.o -t $(V_LIB)/$(RDEST)/
	install -m 0600 $(RDEST)/stub_srvc.o -t $(V_LIB)/$(RDEST)/
	install -m 0600 $(RDEST)/stub_pcre2.o -t $(V_LIB)/$(RDEST)/
	install -m 0600 $(RDEST)/stub_curl.o -t $(V_LIB)/$(RDEST)/
	install -m 0600 $(RDEST)/stub_arr.o -t $(V_LIB)/$(RDEST)/
	install -m 0600 $(RDEST)/stub_tree.o -t $(V_LIB)/$(RDEST)/
	install -m 0600 $(RDEST)/stub_crypto.o -t $(V_LIB)/$(RDEST)/
	install -m 0600 $(RDEST)/stub_before.o -t $(V_LIB)/$(RDEST)/
	install -m 0600 $(RDEST)/stub_after.o -t $(V_LIB)/$(RDEST)/
	install -m 0600 $(RDEST)/stub_xml.o -t $(V_LIB)/$(RDEST)/
	if [ "$(REL)" == "1" ]; then install -m 0700 release/v1 -t $(V_LIB)/
	install -m 0700 release/mrim  -t $(V_BIN)/ 
	install -m 0700 release/rimcli  -t $(V_BIN)/; fi
	$(AT)#the following are copied here and in build_structure. The reason is that RimStone developer can change them
	$(AT)#so they need to be copied here. But when first installing RimStone, they need to be present in order to
	$(AT)#build the rest (well not vmakefile, so it is not in build_structure rule; sys and rim are needed there
	$(AT)#so the library path substitution can be done).
	$(AT)install -D -m 0600 vmakefile -t $(V_LIB)/
	install -D -m 0700 sys -t $(V_LIB)/
	install -D -m 0700 rim  -t $(V_BIN)/
	install -D -m 0700 rimlib  -t $(V_BIN)/
	install -D -m 0600 rim.h -t $(V_INC)/
	install -D -m 0600 rimcommon.h -t $(V_INC)/
	install -D -m 0600 fcgi/fcgiapp.h -t $(V_INC)/
	install -D -m 0600 rcli.h -t $(V_INC)/
	install -D -m 0600 rimmsg.h -t $(V_INC)/
	read -r RIM_C_GCC < <(gcc --version);echo "$$RIM_C_GCC" > $(V_SHARE)/.rim.gcc
	echo "RIM_MARIADB_EX='$(RIM_MARIADB_EX)';RIM_SQLITE_EX='$(RIM_SQLITE_EX)';RIM_CURL_EX='$(RIM_CURL_EX)';RIM_PCRE2_EX='$(RIM_PCRE2_EX)';RIM_CRYPTO_EX='$(RIM_CRYPTO_EX)';RIM_POSTGRES_EX='$(RIM_POSTGRES_EX)';RIM_XML_EX='$(RIM_XML_EX)'" > $(V_SHARE)/.rim.libs
	sed -i "s|^[ ]*export[ ]*RIM_LIBRARY_PATH[ ]*=.*|export RIM_LIBRARY_PATH=$(V_LIB)|g" $(V_LIB)/sys
	sed -i "s|^[ ]*export[ ]*RIM_INCLUDE_PATH[ ]*=.*|export RIM_INCLUDE_PATH=$(V_INC)|g" $(V_LIB)/sys
	sed -i "s|^[ ]*export[ ]*RIM_BIN_PATH[ ]*=.*|export RIM_BIN_PATH=$(V_BIN)|g" $(V_LIB)/sys
	sed -i "s|^[ ]*export[ ]*RIM_VERSION[ ]*=.*|export RIM_VERSION=$(PACKAGE_VERSION)|g" $(V_LIB)/sys
	sed -i "s|^[ ]*export[ ]*RIM_LIBRARY_PATH[ ]*=.*|export RIM_LIBRARY_PATH=$(V_LIB)|g" $(V_BIN)/rim

#Libraries and executables are 0700 
#SELinux directory is created and files put there just so if it ever gets selinux installed
#We only actually enable selinux polices if 1) not a fakeroot and 2) selinux actually installed (doesn't have to be enabled)
#SELinux can be enabled only if DESTDIR is empty, i.e. not a fake root. Otherwise, we're setting policies for fake root files, which of course doesn't work
#Per Debian guidelines, LICENSE file is not installed, as it's already in debian/copyright
#Man pages are installed dh_installman helper for Debian packaging, hence the guard for man page install below
#Man pages update (man-db) runs in any case in install, since it can run in fakeroot or otherwise
#$(RDEST) is the directory where executables go (release or devel)
.PHONY: build_structure
build_structure:
	$(AT)$(info Creating RimStone directories ($(RDEST)))
	$(AT)install -m 0711 -d $(DESTDIR)/ 
	install -m 0711 -d $(V_APPS)/
	install -m 0700 -d $(V_INC)/
	install -m 0700 -d $(V_RIM_ABOUT)/
	install -D -m 0600 rim.h -t $(V_INC)/
	install -D -m 0600 rimcommon.h -t $(V_INC)/
	install -D -m 0600 fcgi/fcgiapp.h -t $(V_INC)/
	install -D -m 0600 rcli.h -t $(V_INC)/
	install -D -m 0600 rimmsg.h -t $(V_INC)/
	install -m 0700 -d $(V_LIB)/
	install -m 0700 -d $(V_LIB)/devel/
	install -m 0700 -d $(V_LIB)/release/
	install -m 0700 -d $(V_LIB)/selinux/
	install -m 0700 -d $(V_SHARE)/
	install -m 0700 -d $(V_SHARE)/src/
	install -m 0700 -d $(V_MAN)/
	install -m 0700 -d $(V_SHARE)/src/
	install -D -m 0600 rim.vim -t $(V_LIB)/
	install -D -m 0600 rim_indent.vim -t $(V_LIB)/
	install -D -m 0700 rimsetenv.sh -t $(V_LIB)/
	install -D -m 0600 LICENSE -t $(V_RIM_ABOUT)/ 
	install -D -m 0600 NOTICE -t $(V_RIM_ABOUT)/
	install -D -m 0600 README.md -t $(V_RIM_ABOUT)/
	install -D -m 0600 CONTRIBUTING -t $(V_RIM_ABOUT)/
	install -m 0700 -d $(V_BIN)/
	install -m 0700 -d $(V_TMP)/
	install -D -m 0700 sys -t $(V_LIB)/
	install -D -m 0700 rimlib  -t $(V_BIN)/
	install -D -m 0700 rim  -t $(V_BIN)/
	$(AT)#install map pages if not a debian package, which does it automatically. man pages will not exist for install of LTO (source recompile in package).
	$(AT)if [ -d docs ]; then install -m 0700 -d $(V_MAN) ; install -D -m 0600 docs/*.2rim -t $(V_MAN)/ ; sed -i "s/\$$VERSION/$(PACKAGE_VERSION)/g" $(V_MAN)/*.2rim; sed -i "s/\$$DATE/$(DATE)/g" $(V_MAN)/*.2rim; fi
	@GF=".version .version_public CONTRIBUTING LICENSE Makefile NOTICE README.md arr.c btrace/LICENSE btrace/atomic.c btrace/backtrace-supported.h btrace/backtrace.c btrace/backtrace.h btrace/config.h btrace/dwarf.c btrace/elf.c btrace/fileline.c btrace/filenames.h btrace/internal.h btrace/mmap.c btrace/mmapio.c btrace/posix.c btrace/print.c btrace/simple.c btrace/sort.c btrace/state.c chandle.c curl.c db.c fcgi/LICENSE fcgi/fastcgi.h fcgi/fcgiapp.c fcgi/fcgiapp.h fcgi/rimsock.c fcgi/rimsock.h fcgi/strerror.c hash.c json.c libsort/CONTRIBUTORS.md libsort/LICENSE.md libsort/sort.h libsort/sort_extra.h lite.c mrim.c msg.c mys.c pcre2.c pg.c rcli.c rcli.h rim rim.h rim.sel rim.te rim.vim rim_indent.vim rimcli.c rimcommon.c rimcommon.h riminst.sh rimlib rimmem.c rimmsg.h rimrt.c rimrtc.c rimsetenv.sh rr.te sec.c sort.c stub.c stub_after.c stub_before.c sys tree.c utf.c utfs.c v1.c vmakefile xml.c "; tar cvfz rimstone.tar.gz $$GF>/dev/null
	$(AT)install -D -m 0600 rimstone.tar.gz -t $(V_SHARE)/ 

#This, if needed, must be run as root (obviously). For instance, if you have SELinux enabled, and you want to run a server that's accessed from say a web server
install-selinux:
	$(AT)$(info Setting up SELinux...)
	$(AT)#This must be last, in this order, as it saves and then applies SELinux policy where applicable. 
	$(AT)#This runs during rpm creation or during make install
	$(AT)#it does NOT run during rpm installation, there is post scriptlet that calls rimstone.sel to do that (RIM_NO_SEL)
	$(AT)install -D -m 0600 rr.te -t $(V_LIB)/selinux/
	$(AT)install -D -m 0600 rim.te -t $(V_LIB)/selinux/
	$(AT)install -D -m 0700 rim.sel -t $(V_LIB)/selinux/
	$(AT)echo '#!/bin/bash'>selinux.setup
	$(AT)echo "if [ -f /etc/selinux/config ]; then $(V_LIB)/selinux/rim.sel '$(V_LIB)/selinux' '$(V_RIM_SELINUXDIR)' '$(DESTDIR)' ; fi">>selinux.setup
	$(AT)chmod 0700 selinux.setup
	$(AT)#this selinux.setup will be executed in rpm post section, and here (below) if this is a system with SELinux
	$(AT)#the text saved to selinux.setup does NOT use DESTDIR, since this script never runs in fake root, meaning if it does run, then DESTDIR is empty (which is
	$(AT)#sudo make install from source) or it runs during %post section when installing package. So in either case, not a fake root.
	$(AT)#This way we also have a script to re-run policy setup if something isn't right
	$(AT)install -D -m 0700 selinux.setup -t $(V_LIB)/selinux
	$(AT)#this has to be double negation because it's purpose is to prevent selinux setup in fake root, which is only from rimstone.spec rpmbuild where RIM_NOSEL is set to 1

#semodule -r rimstone >/dev/null || true  would need to run to remove RimStone associations
#do not delete .rimstone itself, as it will remove selinux labels (restorecon would have to be run)
#delete all but apps which may hold app data
.PHONY: uninstall
uninstall:
	$(AT)echo "Uninstalling RimStone"
	$(AT)if [ -d "$(DESTDIR)" ]; then cd $(DESTDIR); rm -rf about bin rimstone include lib man || true >/dev/null; fi

#this just builds and copies binaries to the directory (based on REL). No install action.
.PHONY: build
build: copy_binaries

#this makes the paths, builds both release and devel, copies the binaries to a directory.
#make install is all it does now (make itself does nothing)
#if REL is not specified, perform make install for the "other"
#release mode (so if this is devel, perform release; and vice versa).
.PHONY: install
install:  
	$(AT)$(MAKE) copy_binaries REL=0 
	$(AT)$(MAKE) copy_binaries REL=1
	$(AT)if [ -f /etc/selinux/config ]; then $(MAKE) install-selinux REL=1; fi

.PHONY: clean
clean:
	$(AT)echo "Cleaning up RimStone for build."
	$(AT)touch *.c
	$(AT)touch *.h
	$(AT)rm -rf *.tar.gz
	$(AT)rm -f devel/*.[oa] devel/btrace/*.[oa] release/*.[oa] release/btrace/*.[oa] devel/fcgi/*.[oa] release/fcgi/*.[oa]
	$(AT)rm -f mrim v1 rimcli
	$(AT)rm -f selinux.setup
	$(AT)rm -f .rimlib.mk



.PHONY: build_spec_all
build_spec_all: 
	$(AT)$(MAKE) REL=0 build_spec
	$(AT)$(MAKE) REL=1 build_spec


RRDEPS=$(RDEST)/v1 $(RDEST)/mrim $(RDEST)/libsrvcrim.a $(RDEST)/librim.a $(RDEST)/librimdb.a $(RDEST)/librimarr.a $(RDEST)/librimarrsort.a $(RDEST)/librimtree.a $(RDEST)/stub_after.o $(RDEST)/stub_before.o $(RDEST)/stub_crypto.o $(RDEST)/stub_xml.o $(RDEST)/stub_curl.o $(RDEST)/stub_arr.o $(RDEST)/stub_tree.o $(RDEST)/stub_pcre2.o $(RDEST)/stub_srvc.o $(RDEST)/stub_sqlite.o $(RDEST)/stub_postgres.o $(RDEST)/stub_mariadb.o $(RDEST)/stub_gendb.o $(RDEST)/librimcli.a $(RDEST)/librimscli.a $(RDEST)/librimbtrace.a $(RDEST)/librimfcgi.a $(RDEST)/rimcli
ifeq ($(RIM_MARIADB_EX),0)
    RRDEPS+=$(RDEST)/librimmys.a
endif
ifeq ($(RIM_POSTGRES_EX),0)
    RRDEPS+=$(RDEST)/librimpg.a
endif
ifeq ($(RIM_SQLITE_EX),0)
    RRDEPS+=$(RDEST)/librimlite.a
endif
ifeq ($(RIM_CURL_EX),0)
    RRDEPS+=$(RDEST)/librimcurl.a
endif
ifeq ($(RIM_PCRE2_EX),0)
    RRDEPS+=$(RDEST)/librimpcre2.a
endif
ifeq ($(RIM_XML_EX),0)
    RRDEPS+=$(RDEST)/librimxml.a
endif
ifeq ($(RIM_CRYPTO_EX),0)
    RRDEPS+=$(RDEST)/librimcrypto.a
endif

.PHONY: build_spec
#v1 doesn't know about rim_finished_output, it always outputs; the distinction is only server vs command-line app
build_spec: $(RRDEPS)
	$(AT)$(info Built RimStone binaries ($(RDEST)))
$(RDEST)/v1: $(RDEST)/v1.o $(RDEST)/rimmems.o $(RDEST)/chandlesrv.o $(RDEST)/rimrtc.o $(RDEST)/hash.o $(RDEST)/utf.o $(RDEST)/librimbtrace.a $(RDEST)/rimcommon.o
	$(AT)echo -n "."
	$(AT)$(CC) -o $@ $^ $(LFLAGS) 
$(RDEST)/v1.o: v1.c rim.h 
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(RDEST)/mrim: $(RDEST)/mrim.o $(RDEST)/librimbtrace.a $(RDEST)/rimcommon.o
	$(AT)echo -n "."
	$(AT)$(CC) -o $@ $^ $(LFLAGS) 
$(RDEST)/libsrvcrim.a: $(RDEST)/chandlesrv.o $(RDEST)/hash.o $(RDEST)/json.o $(RDEST)/msg.o $(RDEST)/utf.o $(RDEST)/utfs.o $(RDEST)/srvc_rimrt.o $(RDEST)/rimrtc.o $(RDEST)/rimmems.o $(RDEST)/rimmemcommon.o
	$(AT)echo -n "."
	$(AT)rm -f $@
	$(AT)$(CCAR) $@ $^ 
$(RDEST)/librim.a: $(RDEST)/chandle.o $(RDEST)/hash.o $(RDEST)/json.o $(RDEST)/msg.o $(RDEST)/utf.o $(RDEST)/utfs.o $(RDEST)/rimrt.o $(RDEST)/rimrtc.o $(RDEST)/rimmems.o $(RDEST)/rimmemcommon.o
	$(AT)echo -n "."
	$(AT)rm -f $@
	$(AT)$(CCAR) -o $@ $^ 
$(RDEST)/librimpg.a: $(RDEST)/pg.o 
	$(AT)rm -f $@
	$(AT)$(CCAR) $@ $^ 
$(RDEST)/librimlite.a: $(RDEST)/lite.o 
	$(AT)echo -n "."
	$(AT)rm -f $@
	$(AT)$(CCAR) $@ $^ 
$(RDEST)/librimmys.a: $(RDEST)/mys.o 
	$(AT)echo -n "."
	$(AT)rm -f $@
	$(AT)$(CCAR) $@ $^ 
$(RDEST)/librimdb.a: $(RDEST)/db.o 
	$(AT)echo -n "."
	$(AT)rm -f $@
	$(AT)$(CCAR) $@ $^ 
$(RDEST)/librimcrypto.a: $(RDEST)/sec.o 
	$(AT)echo -n "."
	$(AT)rm -f $@
	$(AT)$(CCAR) $@  $^ 
$(RDEST)/librimxml.a: $(RDEST)/xml.o 
	$(AT)echo -n "."
	$(AT)rm -f $@
	$(AT)$(CCAR) $@ $^ 
$(RDEST)/librimarrsort.a: $(RDEST)/arr_string_sort.o  $(RDEST)/arr_number_sort.o $(RDEST)/arr_double_sort.o $(RDEST)/arr_string_sort_desc.o  $(RDEST)/arr_number_sort_desc.o $(RDEST)/arr_double_sort_desc.o
	$(AT)echo -n "."
	$(AT)rm -f $@
	$(AT)$(CCAR) $@ $^ 
$(RDEST)/librimarr.a: $(RDEST)/arr_string.o  $(RDEST)/arr_number.o $(RDEST)/arr_bool.o $(RDEST)/arr_double.o
	$(AT)echo -n "."
	$(AT)rm -f $@
	$(AT)$(CCAR) $@ $^ 
$(RDEST)/librimcurl.a: $(RDEST)/curl.o 
	$(AT)echo -n "."
	$(AT)rm -f $@
	$(AT)$(CCAR) $@ $^ 
$(RDEST)/librimtree.a: $(RDEST)/tree.o 
	$(AT)echo -n "."
	$(AT)rm -f $@
	$(AT)$(CCAR) $@ $^ 
$(RDEST)/librimpcre2.a: $(RDEST)/pcre2.o 
	$(AT)echo -n "."
	$(AT)rm -f $@
	$(AT)$(CCAR) $@ $^ 
$(RDEST)/utf.o: utf.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(RDEST)/utfs.o: utfs.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(RDEST)/hash.o: hash.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(RDEST)/json.o: json.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(RDEST)/msgcli.o: msg.c rimcommon.h rimmsg.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -DRIM_MSG_CLI -o $@ $< $(CFLAGS) 
$(RDEST)/msg.o: msg.c rim.h rimmsg.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(RDEST)/tree.o: tree.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(RDEST)/chandle.o: chandle.c rim.h 
	$(AT)echo -n "."
	$(AT)$(CC) -c -DRIM_COMMAND -o $@ $< $(CFLAGS) 
$(RDEST)/chandlesrv.o: chandle.c rim.h 
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(RDEST)/pg.o: pg.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(RDEST)/db.o: db.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(RDEST)/lite.o: lite.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS)
$(RDEST)/mys.o: mys.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS)
$(RDEST)/sec.o: sec.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(RDEST)/stub_after.o: stub_after.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS)
$(RDEST)/stub_before.o: stub_before.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS)
$(RDEST)/stub_crypto.o: stub.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ -DRIM_CRYPTO $< $(CFLAGS) 
$(RDEST)/stub_xml.o: stub.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ -DRIM_XML $< $(CFLAGS) 
$(RDEST)/stub_curl.o: stub.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ -DRIM_CURL $< $(CFLAGS)
$(RDEST)/stub_arr.o: stub.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ -DRIM_ARR $< $(CFLAGS)
$(RDEST)/stub_tree.o: stub.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ -DRIM_TREE $< $(CFLAGS) 
$(RDEST)/stub_pcre2.o: stub.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ -DRIM_PCRE2 $< $(CFLAGS) 
$(RDEST)/stub_srvc.o: stub.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ -DRIM_SERVICE $< $(CFLAGS) 
$(RDEST)/stub_sqlite.o: stub.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ -DRIM_STUB_SQLITE $< $(CFLAGS) 
$(RDEST)/stub_postgres.o: stub.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ -DRIM_STUB_POSTGRES $< $(CFLAGS) 
$(RDEST)/stub_mariadb.o: stub.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ -DRIM_STUB_MARIADB $< $(CFLAGS) 
$(RDEST)/stub_gendb.o: stub.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ -DRIM_STUB_GENDB $< $(CFLAGS) 
$(RDEST)/curl.o: curl.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(RDEST)/xml.o: xml.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(RDEST)/arr_string_sort_desc.o: sort.c rimcommon.h libsort/sort.h libsort/sort_extra.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ sort.c $(CFLAGS) -DRIM_SORT_DESC -DSORT_EXTRA -Wno-missing-declarations -DRIM_ARR_STRING
$(RDEST)/arr_double_sort_desc.o: sort.c rimcommon.h libsort/sort.h libsort/sort_extra.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ sort.c $(CFLAGS) -DRIM_SORT_DESC -DSORT_EXTRA -Wno-missing-declarations -DRIM_ARR_DOUBLE
$(RDEST)/arr_number_sort_desc.o: sort.c rimcommon.h libsort/sort.h libsort/sort_extra.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ sort.c $(CFLAGS) -DRIM_SORT_DESC -DSORT_EXTRA -Wno-missing-declarations -DRIM_ARR_NUMBER
$(RDEST)/arr_string_sort.o: sort.c rimcommon.h libsort/sort.h libsort/sort_extra.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ sort.c $(CFLAGS) -DSORT_EXTRA -Wno-missing-declarations -DRIM_ARR_STRING
$(RDEST)/arr_double_sort.o: sort.c rimcommon.h libsort/sort.h libsort/sort_extra.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ sort.c $(CFLAGS) -DSORT_EXTRA -Wno-missing-declarations -DRIM_ARR_DOUBLE
$(RDEST)/arr_number_sort.o: sort.c rimcommon.h libsort/sort.h libsort/sort_extra.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ sort.c $(CFLAGS) -DSORT_EXTRA -Wno-missing-declarations -DRIM_ARR_NUMBER
$(RDEST)/arr_string.o: arr.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) -DRIM_ARR_STRING
$(RDEST)/arr_double.o: arr.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) -DRIM_ARR_DOUBLE
$(RDEST)/arr_number.o: arr.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) -DRIM_ARR_NUMBER
$(RDEST)/arr_bool.o: arr.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) -DRIM_ARR_BOOL
$(RDEST)/pcre2.o: pcre2.c rim.h
	$(AT)echo -n "."
	$(AT)NEWPCRE2=$$(./sys greater_than_eq "$(PCRE2_VER)" "10.37"); if [ "$$NEWPCRE2" == "0" ]; then GLIBC_REGEX="-DRIM_C_GLIBC_REGEX"; fi ; $(CC) -c -o $@ $< $$GLIBC_REGEX $(CFLAGS) 
$(RDEST)/rimrtc.o: rimrtc.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS)
$(RDEST)/srvc_rimrt.o: rimrt.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS)  
$(RDEST)/rimrt.o: rimrt.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -DRIM_COMMAND -o $@ $< $(CFLAGS)  
$(RDEST)/rimmems.o: rimmem.c rim.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(RDEST)/mrim.o: mrim.c rimcommon.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(RDEST)/librimcli.a: $(RDEST)/rcli.o $(RDEST)/msgcli.o $(RDEST)/rimcommon.o
	$(AT)echo -n "."
	$(AT)rm -f $@
	$(AT)$(CCAR) $@ $^ 
$(RDEST)/rimmemcommon.o: rimcommon.c rimcommon.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) -DRIM_RIMMEM=1
$(RDEST)/rimcommon.o: rimcommon.c rimcommon.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(RDEST)/rcli.o: rcli.c rim.h rcli.h rimcommon.h rimmsg.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS)
$(RDEST)/librimscli.a: $(RDEST)/srvrcli.o $(RDEST)/rimmemcommon.o
	$(AT)echo -n "."
	$(AT)rm -f $@
	$(AT)$(CCAR) $@ $^ 
$(RDEST)/srvrcli.o: rcli.c rim.h rcli.h rimcommon.h rimmsg.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) -DRIM_RIMMEM=1
$(RDEST)/librimfcgi.a: $(RDEST)/fcgi/fcgiapp.o $(RDEST)/fcgi/rimsock.o
	$(AT)echo -n "."
	$(AT)rm -f $@
	$(AT)$(CCAR) $@ $^ 
$(RDEST)/fcgi/fcgiapp.o: fcgi/fcgiapp.c fcgi/fastcgi.h	fcgi/fcgiapp.h	fcgi/rimsock.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@  $< $(CFLAGS) 
$(RDEST)/fcgi/rimsock.o: fcgi/rimsock.c fcgi/fastcgi.h	fcgi/fcgiapp.h	fcgi/rimsock.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@  $< $(CFLAGS) 
$(RDEST)/rimcli: rimcli.c rcli.c $(RDEST)/librimcli.a
	$(AT)echo -n "."
	$(AT)$(CC) -o $@  $<  $(CFLAGS) $(LFLAGS) $(RDEST)/librimcli.a

#libbacktrace, the code bellow is touchy. It build the lib from all source code from btrace dir.
define make_btrace
	$(AT)gcc -DHAVE_CONFIG_H -I. -funwind-tables -frandom-seed=$@ -W -Wall -Wwrite-strings -Wstrict-prototypes -Wmissing-prototypes -Wold-style-definition -Wmissing-format-attribute -Wcast-qual -Wno-attributes -Wpointer-arith -g -O2 -c $< -o $@ $(LTO)
endef
BTRACE_SRC=$(wildcard btrace/*.c)
BOBJ=$(RDEST)/btrace
#BTRACE_OBJ=$(pathsubst btrace/%.c,$(BOBJ)/%.o,$(BTRACE_SRC)) 
BTRACE_OBJ=$(BTRACE_SRC:btrace/%.c=$(BOBJ)/%.o) 
$(RDEST)/btrace/%.o: btrace/%.c $(wildcard btrace/*.h)
	$(call make_btrace)
$(RDEST)/librimbtrace.a: $(BTRACE_OBJ)
	$(AT)rm -f $@
	$(AT)$(CCAR) $@ $^ 

