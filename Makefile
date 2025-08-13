#SPDX-License-Identifier: Apache-2.0
#Copyright 2018-2025 Gliim LLC.  
#Licensed under Apache License v2. See LICENSE file.
#On the web http://golf-lang.com/ - this file is part of Golf framework.

#makefile for Golf. This file builds and installs Golf. sudo make install does it all, though you can do make first. make clean cleans it out.

.ONESHELL:
SHELL:=/bin/bash
ifeq ($(shell id -u), 0)
    $(error You cannot run this makefile as root or sudo)
endif

#this works even if sudo'd (unlike $USER)
GGUSER=$(shell whoami) 
export GGUSER

#
#check DESTDIR, if not specified, default is $HOME/.golf
ifeq ("$(HOME)",)
    HOME=$(shell eval echo ~$(GGUSER))
endif
ifeq ($(strip $(DESTDIR)),)
	DESTDIR:=$(HOME)/.golf
    RES=$(shell install -m 0711 -d $(DESTDIR))
endif
#For socket access, minimum of 711 home directory is required
RES=$(shell chmod 711 $(HOME))
#

#use all CPUs if not specified
MJOBS=$(shell nproc) 
#check if -j or --jobs is there, if not add it
ifndef MAKEFLAGS
    MAKEFLAGS:=-j$(NUM_JOBS)
else ifeq ($(findstring --jobs,$(MAKEFLAGS)),) 
    ifeq ($(findstring -j,$(MAKEFLAGS)),) 
        MAKEFLAGS:=$(MAKEFLAGS) -j$(MJOBS)
    endif
endif
MAKEFLAGS += --no-print-directory

#verbose, show all
ifeq ($(V),1)
    AT:=
else
    AT:=@
endif

#include DEBUG flag, which includes code that normally never executes, even in Devel; this is for Golf developers only
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
    GDEST=release
    GG_ALWAYS_INLINE=__attribute__((always_inline))
    ASAN=
else
    CFLAGS_WARN_ERROR=-Werror 
    OPTIM_COMP=-g3 $(DBG) -rdynamic
    OPTIM_LINK=-rdynamic
    LTO=
    GDEST=devel
    GG_ALWAYS_INLINE=
endif

#determine which libraries are present and which are not (0 means present)
$(shell source ./gglib && \
    echo "" > .gglib.mk && \
    echo "GG_MARIADB_EX:=$$GG_MARIADB_EX" >> .gglib.mk && \
    echo "GG_SQLITE_EX:=$$GG_SQLITE_EX" >> .gglib.mk && \
    echo "GG_CURL_EX:=$$GG_CURL_EX" >> .gglib.mk && \
    echo "GG_PCRE2_EX:=$$GG_PCRE2_EX" >> .gglib.mk && \
    echo "GG_CRYPTO_EX:=$$GG_CRYPTO_EX" >> .gglib.mk && \
    echo "GG_POSTGRES_EX:=$$GG_POSTGRES_EX" >> .gglib.mk && \
    echo "GG_XML_EX:=$$GG_XML_EX" >> .gglib.mk && \
    echo "GG_MARIADB_INC:=$$GG_MARIADB_INC" >> .gglib.mk && \
    echo "GG_POSTGRES_INC:=$$GG_POSTGRES_INC" >> .gglib.mk )
include .gglib.mk


#get pcre2 version and libs
ifeq ($(GG_PCRE2_EX),0)
    PCRE2_VER:=$(shell pcre2-config --version)
endif
#get flags whether to include 3rd libs .h files (based on whether they exist or not)
GGINCF=
ifeq ($(GG_MARIADB_EX),0)
    GGINCF+= -DGG_INC_MARIADB=1
endif
ifeq ($(GG_POSTGRES_EX),0)
    GGINCF+= -DGG_INC_POSTGRES=1
endif
ifeq ($(GG_SQLITE_EX),0)
    GGINCF+= -DGG_INC_SQLITE=1
endif
ifeq ($(GG_CURL_EX),0)
    GGINCF+= -DGG_INC_CURL=1
endif
ifeq ($(GG_CRYPTO_EX),0)
    GGINCF+= -DGG_INC_CRYPTO=1
endif
ifeq ($(GG_XML_EX),0)
    GGINCF+= -DGG_INC_XML=1
endif
ifeq ($(GG_PCRE2_EX),0)
    GGINCF+= -DGG_INC_PCRE2=1
endif
#these must be the same (GG_PLATFORM_ID,GG_PLATFORM_VERSION) used in sys
OSNAME:=$(shell . ./sys; echo -n $${GG_PLATFORM_ID})
OSVERSION:=$(shell . ./sys; echo -n $${GG_PLATFORM_VERSION})

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
V_MAN:=$(DESTDIR)/man/man2
V_SHARE:=$(DESTDIR)/golf
V_GG_SELINUXDIR:=/usr/share
V_GG_ABOUT:=$(DESTDIR)/about


#XML flags
GG_LIBXML2_INCLUDE:=$(shell pkgconf --silence-errors --cflags libxml-2.0)

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
CFLAGS=$(CFLAGS_WARN_ERROR) -Wall -Wextra -Wuninitialized -Wmissing-declarations -Wformat -Werror=format-security -Wno-format-zero-length -funsigned-char -fpic -fno-semantic-interposition  $(GG_MARIADB_INC) $(GG_POSTGRES_INC) $(GG_LIBXML2_INCLUDE) -DGG_OSNAME="\"$(OSNAME)\"" -DGG_OSVERSION="\"$(OSVERSION)\"" -DGG_PKGVERSION="\"$(PACKAGE_VERSION)\"" -DGG_ALWAYS_INLINE='$(GG_ALWAYS_INLINE)' $(OPTIM_COMP) $(ASAN) $(PROF) -fmax-errors=5 -Wdate-time -fno-stack-protector -fno-stack-clash-protection $(LTO) -funwind-tables $(GGINCF)

#linker flags. 
LFLAGS_COMMON=-Wl,-z,relro
LFLAGS=-Wl,-rpath=$(V_LIB)/$(GDEST) -Wl,--enable-new-dtags -L$(V_LIB)/$(GDEST) $(OPTIM_LINK) $(LFLAGS_COMMON) $(ASAN) $(PROF) $(LTO)

nothing: 
	$(AT)echo "Golf $(PACKAGE_VERSION) development build complete."

#Copy binaries from building area to their final place. $(GDEST) is the directory (release or devel)
#no -D in install, we want it to fail if no directories; use make install for that first.
#build_spec builds into local release/devel dir; build_structure builds install folders; this copies former into the latter.
#The order doesn't matter since the two are independent folders.
copy_binaries: build_spec  build_structure
	$(AT)$(info Copying Golf binaries ($(GDEST)))
	$(AT)install -m 0700 $(GDEST)/libggfcgi.a -t $(V_LIB)/$(GDEST)/
	install -m 0700 $(GDEST)/libgolfbtrace.a -t $(V_LIB)/$(GDEST)/
	if [ "$(GG_POSTGRES_EX)" == "0" ]; then install -m 0700 $(GDEST)/libgolfpg.a -t $(V_LIB)/$(GDEST)/ ; fi
	install -m 0700 $(GDEST)/libgolfdb.a -t $(V_LIB)/$(GDEST)/
	if [ "$(GG_SQLITE_EX)" == "0" ]; then install -m 0700 $(GDEST)/libgolflite.a -t $(V_LIB)/$(GDEST)/ ; fi
	if [ "$(GG_MARIADB_EX)" == "0" ]; then install -m 0700 $(GDEST)/libgolfmys.a -t $(V_LIB)/$(GDEST)/ ; fi
	if [ "$(GG_CRYPTO_EX)" == "0" ]; then install -m 0700 $(GDEST)/libgolfcrypto.a -t $(V_LIB)/$(GDEST)/ ; fi
	install -m 0700 $(GDEST)/libgolftree.a -t $(V_LIB)/$(GDEST)/
	if [ "$(GG_CURL_EX)" == "0" ]; then install -m 0700 $(GDEST)/libgolfcurl.a -t $(V_LIB)/$(GDEST)/ ; fi
	if [ "$(GG_XML_EX)" == "0" ]; then install -m 0700 $(GDEST)/libgolfxml.a -t $(V_LIB)/$(GDEST)/ ; fi
	install -m 0700 $(GDEST)/libgolfarr.a -t $(V_LIB)/$(GDEST)/
	if [ "$(GG_PCRE2_EX)" == "0" ]; then install -m 0700 $(GDEST)/libgolfpcre2.a -t $(V_LIB)/$(GDEST)/ ; fi
	install -m 0700 $(GDEST)/libsrvcgolf.a -t $(V_LIB)/$(GDEST)/
	install -m 0700 $(GDEST)/libgolf.a -t $(V_LIB)/$(GDEST)/
	install -m 0700 $(GDEST)/libgolfcli.a -t $(V_LIB)/$(GDEST)/
	install -m 0700 $(GDEST)/libgolfscli.a -t $(V_LIB)/$(GDEST)/
	install -m 0600 $(GDEST)/stub_sqlite.o -t $(V_LIB)/$(GDEST)/
	install -m 0600 $(GDEST)/stub_postgres.o -t $(V_LIB)/$(GDEST)/
	install -m 0600 $(GDEST)/stub_mariadb.o -t $(V_LIB)/$(GDEST)/
	install -m 0600 $(GDEST)/stub_gendb.o -t $(V_LIB)/$(GDEST)/
	install -m 0600 $(GDEST)/stub_srvc.o -t $(V_LIB)/$(GDEST)/
	install -m 0600 $(GDEST)/stub_pcre2.o -t $(V_LIB)/$(GDEST)/
	install -m 0600 $(GDEST)/stub_curl.o -t $(V_LIB)/$(GDEST)/
	install -m 0600 $(GDEST)/stub_arr.o -t $(V_LIB)/$(GDEST)/
	install -m 0600 $(GDEST)/stub_tree.o -t $(V_LIB)/$(GDEST)/
	install -m 0600 $(GDEST)/stub_crypto.o -t $(V_LIB)/$(GDEST)/
	install -m 0600 $(GDEST)/stub_before.o -t $(V_LIB)/$(GDEST)/
	install -m 0600 $(GDEST)/stub_after.o -t $(V_LIB)/$(GDEST)/
	install -m 0600 $(GDEST)/stub_xml.o -t $(V_LIB)/$(GDEST)/
	if [ "$(REL)" == "1" ]; then install -m 0700 release/v1 -t $(V_LIB)/
	install -m 0700 release/mgrg  -t $(V_BIN)/ 
	install -m 0700 release/ggcli  -t $(V_LIB)/; fi
	$(AT)#the following are copied here and in build_structure. The reason is that Golf developer can change them
	$(AT)#so they need to be copied here. But when first installing Golf, they need to be present in order to
	$(AT)#build the rest (well not vmakefile, so it is not in build_structure rule; sys and gg are needed there
	$(AT)#so the library path substitution can be done).
	$(AT)install -D -m 0600 vmakefile -t $(V_LIB)/
	install -D -m 0700 sys -t $(V_LIB)/
	install -D -m 0700 gg  -t $(V_BIN)/
	install -D -m 0700 gglib  -t $(V_BIN)/
	install -D -m 0600 golf.h -t $(V_INC)/
	install -D -m 0600 ggcommon.h -t $(V_INC)/
	install -D -m 0600 fcgi/fcgiapp.h -t $(V_INC)/
	install -D -m 0600 gcli.h -t $(V_INC)/
	read -r GG_C_GCC < <(gcc --version);echo "$$GG_C_GCC" > $(V_SHARE)/.golf.gcc
	echo "GG_MARIADB_EX='$(GG_MARIADB_EX)';GG_SQLITE_EX='$(GG_SQLITE_EX)';GG_CURL_EX='$(GG_CURL_EX)';GG_PCRE2_EX='$(GG_PCRE2_EX)';GG_CRYPTO_EX='$(GG_CRYPTO_EX)';GG_POSTGRES_EX='$(GG_POSTGRES_EX)';GG_XML_EX='$(GG_XML_EX)'" > $(V_SHARE)/.golf.libs
	sed -i "s|^[ ]*export[ ]*GG_LIBRARY_PATH[ ]*=.*|export GG_LIBRARY_PATH=$(V_LIB)|g" $(V_LIB)/sys
	sed -i "s|^[ ]*export[ ]*GG_INCLUDE_PATH[ ]*=.*|export GG_INCLUDE_PATH=$(V_INC)|g" $(V_LIB)/sys
	sed -i "s|^[ ]*export[ ]*GG_BIN_PATH[ ]*=.*|export GG_BIN_PATH=$(V_BIN)|g" $(V_LIB)/sys
	sed -i "s|^[ ]*export[ ]*GG_VERSION[ ]*=.*|export GG_VERSION=$(PACKAGE_VERSION)|g" $(V_LIB)/sys
	sed -i "s|^[ ]*export[ ]*GG_LIBRARY_PATH[ ]*=.*|export GG_LIBRARY_PATH=$(V_LIB)|g" $(V_BIN)/gg

#Libraries and executables are 0700 
#SELinux directory is created and files put there just so if it ever gets selinux installed
#We only actually enable selinux polices if 1) not a fakeroot and 2) selinux actually installed (doesn't have to be enabled)
#SELinux can be enabled only if DESTDIR is empty, i.e. not a fake root. Otherwise, we're setting policies for fake root files, which of course doesn't work
#Per Debian guidelines, LICENSE file is not installed, as it's already in debian/copyright
#Man pages are installed dh_installman helper for Debian packaging, hence the guard for man page install below
#Man pages update (man-db) runs in any case in install, since it can run in fakeroot or otherwise
#$(GDEST) is the directory where executables go (release or devel)
.PHONY: build_structure
build_structure:
	$(AT)$(info Creating Golf directories($(GDEST)))
	$(AT)install -m 0711 -d $(DESTDIR)/ 
	install -m 0711 -d $(V_APPS)/
	install -m 0700 -d $(V_INC)/
	install -m 0700 -d $(V_GG_ABOUT)/
	install -D -m 0600 golf.h -t $(V_INC)/
	install -D -m 0600 ggcommon.h -t $(V_INC)/
	install -D -m 0600 fcgi/fcgiapp.h -t $(V_INC)/
	install -D -m 0600 gcli.h -t $(V_INC)/
	install -m 0700 -d $(V_LIB)/
	install -m 0700 -d $(V_LIB)/devel/
	install -m 0700 -d $(V_LIB)/release/
	install -m 0700 -d $(V_LIB)/selinux/
	install -m 0700 -d $(V_SHARE)/
	install -m 0700 -d $(V_SHARE)/src/
	install -m 0700 -d $(V_MAN)/
	install -m 0700 -d $(V_SHARE)/src/
	install -D -m 0600 golf.vim -t $(V_LIB)/
	install -D -m 0600 golf_indent.vim -t $(V_LIB)/
	install -D -m 0600 LICENSE -t $(V_GG_ABOUT)/ 
	install -D -m 0600 NOTICE -t $(V_GG_ABOUT)/
	install -D -m 0600 README -t $(V_GG_ABOUT)/
	install -D -m 0600 CONTRIBUTING -t $(V_GG_ABOUT)/
	install -m 0700 -d $(V_BIN)/
	install -D -m 0700 sys -t $(V_LIB)/
	install -D -m 0700 gglib  -t $(V_BIN)/
	install -D -m 0700 gg  -t $(V_BIN)/
	$(AT)#install map pages if not a debian package, which does it automatically. man pages will not exist for install of LTO (source recompile in package).
	$(AT)if [ -d docs ]; then install -m 0700 -d $(V_MAN) ; install -D -m 0600 docs/*.2gg -t $(V_MAN)/ ; sed -i "s/\$$VERSION/$(PACKAGE_VERSION)/g" $(V_MAN)/*.2gg; sed -i "s/\$$DATE/$(DATE)/g" $(V_MAN)/*.2gg; fi
	@GF=".version .version_public CONTRIBUTING LICENSE Makefile NOTICE README arr.c btrace/LICENSE btrace/atomic.c btrace/backtrace-supported.h btrace/backtrace.c btrace/backtrace.h btrace/config.h btrace/dwarf.c btrace/elf.c btrace/fileline.c btrace/filenames.h btrace/internal.h btrace/mmap.c btrace/mmapio.c btrace/posix.c btrace/print.c btrace/simple.c btrace/sort.c btrace/state.c chandle.c curl.c db.c fcgi/LICENSE fcgi/fastcgi.h fcgi/fcgiapp.c fcgi/fcgiapp.h fcgi/ggsock.c fcgi/ggsock.h fcgi/strerror.c gcli.c gcli.h gg gg.te ggcli.c ggcommon.c ggcommon.h gginst.sh gglib golf.h golf.sel golf.te golf.vim golf_indent.vim golfmem.c golfrt.c golfrtc.c hash.c json.c lite.c mgrg.c msg.c mys.c pcre2.c pg.c sec.c stub.c stub_after.c stub_before.c sys tree.c utf.c utfs.c v1.c vmakefile xml.c"; tar cvfz golf.tar.gz $$GF >/dev/null;
	$(AT)install -D -m 0600 golf.tar.gz -t $(V_SHARE)/ 

#This, if needed, must be run as root (obviously). For instance, if you have SELinux enabled, and you want to run a server that's accessed from say a web server
install-selinux:
	$(AT)$(info Setting up SELinux...)
	$(AT)#This must be last, in this order, as it saves and then applies SELinux policy where applicable. 
	$(AT)#This runs during rpm creation or during make install
	$(AT)#it does NOT run during rpm installation, there is post scriptlet that calls golf.sel to do that (GG_NO_SEL)
	$(AT)install -D -m 0600 gg.te -t $(V_LIB)/selinux/
	install -D -m 0600 golf.te -t $(V_LIB)/selinux/
	install -D -m 0700 golf.sel -t $(V_LIB)/selinux/
	echo '#!/bin/bash'>selinux.setup
	echo "if [ -f /etc/selinux/config ]; then $(V_LIB)/selinux/golf.sel '$(V_LIB)/selinux' '$(V_GG_SELINUXDIR)' '$(DESTDIR)' ; fi">>selinux.setup
	chmod 0700 selinux.setup
	$(AT)#this selinux.setup will be executed in rpm post section, and here (below) if this is a system with SELinux
	$(AT)#the text saved to selinux.setup does NOT use DESTDIR, since this script never runs in fake root, meaning if it does run, then DESTDIR is empty (which is
	$(AT)#sudo make install from source) or it runs during %post section when installing package. So in either case, not a fake root.
	$(AT)#This way we also have a script to re-run policy setup if something isn't right
	$(AT)install -D -m 0700 selinux.setup -t $(V_LIB)/selinux
	$(AT)#this has to be double negation because it's purpose is to prevent selinux setup in fake root, which is only from golf.spec rpmbuild where GG_NOSEL is set to 1

#semodule -r golf >/dev/null || true  would need to run to remove Golf associations
#do not delete .golf itself, as it will remove selinux labels (restorecon would have to be run)
.PHONY: uninstall
uninstall:
	$(AT)echo "Uninstalling Golf"
	$(AT)rm -rf $(DESTDIR)/* || true >/dev/null

#this just builds and copies binaries to the directory (based on REL). No install action.
.PHONY: build
build: copy_binaries

#this makes the paths, builds both release and devel, copies the binaries to a directory.
#make install is all it does now (make itself does nothing)
#if REL is not specified, perform make install for the "other"
#release mode (so if this is devel, perform release; and vice versa).
.PHONY: install
install:  
	$(AT)make copy_binaries REL=0 -j$(MJOBS)
	$(AT)make copy_binaries REL=1 -j$(MJOBS)
	$(AT)if [ -f /etc/selinux/config ]; then make install-selinux REL=1 -j$(MJOBS); fi

.PHONY: clean
clean:
	$(AT)echo "Cleaning up Golf for build."
	$(AT)touch *.c
	$(AT)touch *.h
	$(AT)rm -rf *.tar.gz
	$(AT)rm -f devel/*.[oa] devel/btrace/*.[oa] release/*.[oa] release/btrace/*.[oa] devel/fcgi/*.[oa] release/fcgi/*.[oa]
	$(AT)rm -f mgrg v1 ggcli
	$(AT)rm -f selinux.setup
	$(AT)rm -f .gglib.mk



.PHONY: build_spec_all
build_spec_all: 
	$(AT)make REL=0 build_spec
	$(AT)make REL=1 build_spec


GGDEPS=$(GDEST)/v1 $(GDEST)/mgrg $(GDEST)/libsrvcgolf.a $(GDEST)/libgolf.a $(GDEST)/libgolfdb.a $(GDEST)/libgolfarr.a $(GDEST)/libgolftree.a $(GDEST)/stub_after.o $(GDEST)/stub_before.o $(GDEST)/stub_crypto.o $(GDEST)/stub_xml.o $(GDEST)/stub_curl.o $(GDEST)/stub_arr.o $(GDEST)/stub_tree.o $(GDEST)/stub_pcre2.o $(GDEST)/stub_srvc.o $(GDEST)/stub_sqlite.o $(GDEST)/stub_postgres.o $(GDEST)/stub_mariadb.o $(GDEST)/stub_gendb.o $(GDEST)/libgolfcli.a $(GDEST)/libgolfscli.a $(GDEST)/libgolfbtrace.a $(GDEST)/libggfcgi.a $(GDEST)/ggcli
ifeq ($(GG_MARIADB_EX),0)
    GGDEPS+=$(GDEST)/libgolfmys.a
endif
ifeq ($(GG_POSTGRES_EX),0)
    GGDEPS+=$(GDEST)/libgolfpg.a
endif
ifeq ($(GG_SQLITE_EX),0)
    GGDEPS+=$(GDEST)/libgolflite.a
endif
ifeq ($(GG_CURL_EX),0)
    GGDEPS+=$(GDEST)/libgolfcurl.a
endif
ifeq ($(GG_PCRE2_EX),0)
    GGDEPS+=$(GDEST)/libgolfpcre2.a
endif
ifeq ($(GG_XML_EX),0)
    GGDEPS+=$(GDEST)/libgolfxml.a
endif
ifeq ($(GG_CRYPTO_EX),0)
    GGDEPS+=$(GDEST)/libgolfcrypto.a
endif

.PHONY: build_spec
#v1 doesn't know about gg_finished_output, it always outputs; the distinction is only server vs command-line app
build_spec: $(GGDEPS)
	$(AT)$(info Building Golf binaries ($(GDEST)))
$(GDEST)/v1: $(GDEST)/v1.o $(GDEST)/golfmems.o $(GDEST)/chandlesrv.o $(GDEST)/golfrtc.o $(GDEST)/hash.o $(GDEST)/utf.o $(GDEST)/libgolfbtrace.a $(GDEST)/ggcommon.o
	$(AT)echo -n "."
	$(AT)$(CC) -o $@ $^ $(LFLAGS) 
$(GDEST)/v1.o: v1.c golf.h 
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(GDEST)/mgrg: $(GDEST)/mgrg.o $(GDEST)/libgolfbtrace.a $(GDEST)/ggcommon.o
	$(AT)echo -n "."
	$(AT)$(CC) -o $@ $^ $(LFLAGS) 
$(GDEST)/libsrvcgolf.a: $(GDEST)/chandlesrv.o $(GDEST)/hash.o $(GDEST)/json.o $(GDEST)/msg.o $(GDEST)/utf.o $(GDEST)/utfs.o $(GDEST)/srvc_golfrt.o $(GDEST)/golfrtc.o $(GDEST)/golfmems.o $(GDEST)/ggmemcommon.o
	$(AT)echo -n "."
	$(AT)rm -f $@
	$(AT)$(CCAR) $@ $^ 
$(GDEST)/libgolf.a: $(GDEST)/chandle.o $(GDEST)/hash.o $(GDEST)/json.o $(GDEST)/msg.o $(GDEST)/utf.o $(GDEST)/utfs.o $(GDEST)/golfrt.o $(GDEST)/golfrtc.o $(GDEST)/golfmems.o $(GDEST)/ggmemcommon.o
	$(AT)echo -n "."
	$(AT)rm -f $@
	$(AT)$(CCAR) -o $@ $^ 
$(GDEST)/libgolfpg.a: $(GDEST)/pg.o 
	$(AT)rm -f $@
	$(AT)$(CCAR) $@ $^ 
$(GDEST)/libgolflite.a: $(GDEST)/lite.o 
	$(AT)echo -n "."
	$(AT)rm -f $@
	$(AT)$(CCAR) $@ $^ 
$(GDEST)/libgolfmys.a: $(GDEST)/mys.o 
	$(AT)echo -n "."
	$(AT)rm -f $@
	$(AT)$(CCAR) $@ $^ 
$(GDEST)/libgolfdb.a: $(GDEST)/db.o 
	$(AT)echo -n "."
	$(AT)rm -f $@
	$(AT)$(CCAR) $@ $^ 
$(GDEST)/libgolfcrypto.a: $(GDEST)/sec.o 
	$(AT)echo -n "."
	$(AT)rm -f $@
	$(AT)$(CCAR) $@  $^ 
$(GDEST)/libgolfxml.a: $(GDEST)/xml.o 
	$(AT)echo -n "."
	$(AT)rm -f $@
	$(AT)$(CCAR) $@ $^ 
$(GDEST)/libgolfarr.a: $(GDEST)/arr_string.o  $(GDEST)/arr_number.o $(GDEST)/arr_bool.o
	$(AT)echo -n "."
	$(AT)rm -f $@
	$(AT)$(CCAR) $@ $^ 
$(GDEST)/libgolfcurl.a: $(GDEST)/curl.o 
	$(AT)echo -n "."
	$(AT)rm -f $@
	$(AT)$(CCAR) $@ $^ 
$(GDEST)/libgolftree.a: $(GDEST)/tree.o 
	$(AT)echo -n "."
	$(AT)rm -f $@
	$(AT)$(CCAR) $@ $^ 
$(GDEST)/libgolfpcre2.a: $(GDEST)/pcre2.o 
	$(AT)echo -n "."
	$(AT)rm -f $@
	$(AT)$(CCAR) $@ $^ 
$(GDEST)/utf.o: utf.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(GDEST)/utfs.o: utfs.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(GDEST)/hash.o: hash.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(GDEST)/json.o: json.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(GDEST)/msg.o: msg.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(GDEST)/tree.o: tree.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(GDEST)/chandle.o: chandle.c golf.h 
	$(AT)echo -n "."
	$(AT)$(CC) -c -DGG_COMMAND -o $@ $< $(CFLAGS) 
$(GDEST)/chandlesrv.o: chandle.c golf.h 
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(GDEST)/pg.o: pg.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(GDEST)/db.o: db.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(GDEST)/lite.o: lite.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS)
$(GDEST)/mys.o: mys.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS)
$(GDEST)/sec.o: sec.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(GDEST)/stub_after.o: stub_after.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS)
$(GDEST)/stub_before.o: stub_before.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS)
$(GDEST)/stub_crypto.o: stub.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ -DGG_CRYPTO $< $(CFLAGS) 
$(GDEST)/stub_xml.o: stub.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ -DGG_XML $< $(CFLAGS) 
$(GDEST)/stub_curl.o: stub.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ -DGG_CURL $< $(CFLAGS)
$(GDEST)/stub_arr.o: stub.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ -DGG_ARR $< $(CFLAGS)
$(GDEST)/stub_tree.o: stub.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ -DGG_TREE $< $(CFLAGS) 
$(GDEST)/stub_pcre2.o: stub.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ -DGG_PCRE2 $< $(CFLAGS) 
$(GDEST)/stub_srvc.o: stub.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ -DGG_SERVICE $< $(CFLAGS) 
$(GDEST)/stub_sqlite.o: stub.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ -DGG_STUB_SQLITE $< $(CFLAGS) 
$(GDEST)/stub_postgres.o: stub.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ -DGG_STUB_POSTGRES $< $(CFLAGS) 
$(GDEST)/stub_mariadb.o: stub.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ -DGG_STUB_MARIADB $< $(CFLAGS) 
$(GDEST)/stub_gendb.o: stub.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ -DGG_STUB_GENDB $< $(CFLAGS) 
$(GDEST)/curl.o: curl.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(GDEST)/xml.o: xml.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(GDEST)/arr_string.o: arr.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) -DGG_ARR_STRING
$(GDEST)/arr_number.o: arr.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) -DGG_ARR_NUMBER
$(GDEST)/arr_bool.o: arr.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) -DGG_ARR_BOOL
$(GDEST)/pcre2.o: pcre2.c golf.h
	$(AT)echo -n "."
	$(AT)NEWPCRE2=$$(./sys greater_than_eq "$(PCRE2_VER)" "10.37"); if [ "$$NEWPCRE2" == "0" ]; then GLIBC_REGEX="-DGG_C_GLIBC_REGEX"; fi ; $(CC) -c -o $@ $< $$GLIBC_REGEX $(CFLAGS) 
$(GDEST)/golfrtc.o: golfrtc.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS)
$(GDEST)/srvc_golfrt.o: golfrt.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS)  
$(GDEST)/golfrt.o: golfrt.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -DGG_COMMAND -o $@ $< $(CFLAGS)  
$(GDEST)/golfmems.o: golfmem.c golf.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(GDEST)/mgrg.o: mgrg.c ggcommon.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(GDEST)/libgolfcli.a: $(GDEST)/gcli.o $(GDEST)/ggcommon.o
	$(AT)echo -n "."
	$(AT)rm -f $@
	$(AT)$(CCAR) $@ $^ 
$(GDEST)/ggmemcommon.o: ggcommon.c ggcommon.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) -DGG_GOLFMEM=1
$(GDEST)/ggcommon.o: ggcommon.c ggcommon.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) 
$(GDEST)/gcli.o: gcli.c golf.h gcli.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS)
$(GDEST)/libgolfscli.a: $(GDEST)/srvgcli.o $(GDEST)/ggmemcommon.o
	$(AT)echo -n "."
	$(AT)rm -f $@
	$(AT)$(CCAR) $@ $^ 
$(GDEST)/srvgcli.o: gcli.c golf.h gcli.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@ $< $(CFLAGS) -DGG_GOLFMEM=1
$(GDEST)/libggfcgi.a: $(GDEST)/fcgi/fcgiapp.o $(GDEST)/fcgi/ggsock.o
	$(AT)echo -n "."
	$(AT)rm -f $@
	$(AT)$(CCAR) $@ $^ 
$(GDEST)/fcgi/fcgiapp.o: fcgi/fcgiapp.c fcgi/fastcgi.h	fcgi/fcgiapp.h	fcgi/ggsock.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@  $< $(CFLAGS) 
$(GDEST)/fcgi/ggsock.o: fcgi/ggsock.c fcgi/fastcgi.h	fcgi/fcgiapp.h	fcgi/ggsock.h
	$(AT)echo -n "."
	$(AT)$(CC) -c -o $@  $< $(CFLAGS) 
$(GDEST)/ggcli: ggcli.c gcli.c $(GDEST)/libgolfcli.a
	$(AT)echo -n "."
	$(AT)$(CC) -o $@  $<  $(CFLAGS) $(LFLAGS) $(GDEST)/libgolfcli.a

#libbacktrace, the code bellow is touchy. It build the lib from all source code from btrace dir.
define make_btrace
	$(AT)gcc -DHAVE_CONFIG_H -I. -funwind-tables -frandom-seed=$@ -W -Wall -Wwrite-strings -Wstrict-prototypes -Wmissing-prototypes -Wold-style-definition -Wmissing-format-attribute -Wcast-qual -Wno-attributes -Wpointer-arith -g -O2 -c $< -o $@ $(LTO)
endef
BTRACE_SRC=$(wildcard btrace/*.c)
BOBJ=$(GDEST)/btrace
#BTRACE_OBJ=$(pathsubst btrace/%.c,$(BOBJ)/%.o,$(BTRACE_SRC)) 
BTRACE_OBJ=$(BTRACE_SRC:btrace/%.c=$(BOBJ)/%.o) 
$(GDEST)/btrace/%.o: btrace/%.c $(wildcard btrace/*.h)
	$(call make_btrace)
$(GDEST)/libgolfbtrace.a: $(BTRACE_OBJ)
	$(AT)rm -f $@
	$(AT)$(CCAR) $@ $^ 

