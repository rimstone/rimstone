#SPDX-License-Identifier: Apache-2.0
#Copyright 2018-2025 Gliim LLC.  
#Licensed under Apache License v2. See LICENSE file.
#On the web http://golf-lang.com/ - this file is part of Golf framework.

#makefile for Golf

.DEFAULT_GOAL=build
SHELL:=/bin/bash
#shortcut for DEBUGINFO
DEBUGINFO=$(DI)
#skip updating of version/date of docs (internal option)
NOUPDOCS=$(ND)
#short cut for Address Sanitizer, internal only
ASAN=$(A)

#get pcre2 version and libs
PCRE2_VER=$(shell pcre2-config --version)
PCRE2_LIBS=$(shell pcre2-config --libs-posix)
#these must be the same (GG_PLATFORM_ID,GG_PLATFORM_VERSION) used in sys
OSNAME=$(shell . ./sys; echo -n $${GG_PLATFORM_ID})
OSVERSION=$(shell . ./sys; echo -n $${GG_PLATFORM_VERSION})
PGCONF=$(shell ./sys pgconf)
DATE=$(shell date +"%b-%d-%Y")
GCC_VER=$(shell gcc -dumpversion)
GG_OS_CLOSE_2=$(shell grep OS_Close /usr/include/fcgios.h|grep shutdown|wc -l)

#these 2 must match application name from config file from each application

CC=gcc

#get build version and release, if .verstion not present, then empty
PACKAGE_VERSION=$(shell . .version || true; echo $${PACKAGE_VERSION})



ifeq ($(strip $(PACKAGE_VERSION)),)
    PACKAGE_VERSION=2
endif

ifeq ($(strip $(GG_LOCAL)),1)
    GG_ROOT=$(DESTDIR)
else
    GG_ROOT=
endif

V_LIB=/usr/lib/golf
V_LIBD=/usr/lib/debug/golf
V_INC=/usr/include/golf
V_BIN=/usr/bin
V_MAN=/usr/share/man/man2

#see if man pages exist (or if not, need reindex)
MANEXIST=$(shell if [ -d "$(V_MAN)" ]; then echo "1"; else echo "0"; fi)


V_GG_DATADIR=/usr/share
V_GG_DATADIR_GOLF=/usr/share/doc/golf

GG_SERVICE_INCLUDE=-I /usr/include/fastcgi

ifeq ($(strip $(PGCONF)),yes)
    GG_POSTGRES_INCLUDE=-I $(shell pg_config --includedir) 
else
    GG_POSTGRES_INCLUDE=$(shell  pkg-config --cflags libpq) 
endif

GG_MARIA_INCLUDE=$(shell mariadb_config --include)
GG_MARIA_LGPLCLIENT_LIBS=$(shell mariadb_config --libs) 
#$(shell mariadb_config --libs_sys)

GG_LIBXML2_INCLUDE=$(shell pkg-config --cflags libxml-2.0)

#based on DEBUGINFO from debug file, we use appropriate tags
#Note: we always use -g in order to get line number of where the problem is
#(optimization is still valid though)
#if DEBUGINFO is 1, then no dbg files will be created, so delete any old ones as they wouldn't be accurate now
ifeq ($(DEBUGINFO),1)
    NONE=$(shell rm -f *.dbg)
    CFLAGS_WARN_ERROR=-Werror 
    OPTIM_COMP=-g3 -DDEBUG -rdynamic
    OPTIM_LINK=-rdynamic
else
    CFLAGS_WARN_ERROR=
    OPTIM_COMP=-g3 -O3
    OPTIM_LINK=
endif
ifeq ($(ASAN),1)
    ASAN=-fsanitize=address -fsanitize-recover=address
else
    ASAN=
endif

ifeq ($(shell test $(GCC_VER) -ge 8; echo $$?),0)
    GCC_FPREFIXMAP=-ffile-prefix-map=$(CURDIR)=.
else
    GCC_FPREFIXMAP=
endif

#C flags are as strict as we can do, in order to discover as many bugs as early on
CFLAGS=$(CFLAGS_WARN_ERROR) -Wall -Wextra -Wuninitialized -Wmissing-declarations -Wformat -Werror=format-security -Wno-format-zero-length -funsigned-char -fpic -fno-semantic-interposition  $(GG_MARIA_INCLUDE) $(GG_POSTGRES_INCLUDE) $(GG_SERVICE_INCLUDE) $(GG_LIBXML2_INCLUDE) -DGG_OSNAME="\"$(OSNAME)\"" -DGG_OSVERSION="\"$(OSVERSION)\"" -DGG_PKGVERSION="\"$(PACKAGE_VERSION)\"" -DGG_ROOT="\"$(GG_ROOT)\"" $(OPTIM_COMP) $(ASAN) -fmax-errors=5 -Wdate-time $(GCC_FPREFIXMAP) 

#linker flags include mariadb (LGPL), crypto (OpenSSL, permissive license). This is for building object code that's part 
#this is for installation at customer's site where we link GOLF with mariadb (LGPL), crypto (OpenSSL)
LFLAGS_COMMON=-Wl,-z,relro
LFLAGS=-Wl,-rpath=$(DESTDIR)$(V_LIB) -Wl,--enable-new-dtags -L$(DESTDIR)$(V_LIB) $(OPTIM_LINK) $(LFLAGS_COMMON) $(ASAN)

#note that for make DI=1, DEBUGINFO can be checked. But we don't specify DEBUGINFO for sudo make install.
#then we wanted to have them all. Otherwise, none will be there, because we explicitly delete them here.
#If debugging build, no need to do anything, the executable is fully endowed with debugging symbols; but if
#not, then separate debugging info, and strip the executable and then link it to debugging info. The only
#exception is if this is debian build which does that (and lintian complains if we strip it ourselves).
define strip_sym
if [ "$(DEBUGINFO)" != "1" ]; then objcopy --only-keep-debug $@ $@.dbg || true ; if [[ "$(GG_DEBIAN_BUILD)" != "1" && "$(GG_FEDORA_BUILD)" != "1" ]]; then objcopy --strip-unneeded $@ || true ; objcopy --add-gnu-debuglink=$@.dbg $@ || true ; else rm -f $@.dbg || true ; fi ; fi
endef


#Libraries and executables must be 0755 or the packager (RPM) will say they are not satisfied
#SELinux directory is created and files put there just so if it ever gets selinux installed
#We only actually enable selinux polices if 1) not a fakeroot and 2) selinux actually installed (doesn't have to be enabled)
#SELinux can be enabled only if DESTDIR is empty, i.e. not a fake root. Otherwise, we're setting policies for fake root files, which of course doesn't work
#Per Debian guidelines, LICENSE file is not installed, as it's already in debian/copyright
#Man pages are installed dh_installman helper for Debian packaging, hence the guard for man page install below
#Man pages update (man-db) runs in any case in install, since it can run in fakeroot or otherwise
.PHONY: install
install:
	install -m 0755 -d $(DESTDIR)/var/lib/gg/bld
	install -m 0755 -d $(DESTDIR)$(V_INC)
	install -m 0755 -d $(DESTDIR)$(V_GG_DATADIR_GOLF)
	install -D -m 0644 golf.h -t $(DESTDIR)$(V_INC)/
	install -D -m 0644 gcli.h -t $(DESTDIR)$(V_INC)/
	install -m 0755 -d $(DESTDIR)$(V_LIB)
	install -m 0755 -d $(DESTDIR)$(V_LIB)/selinux
	install -D -m 0755 libgolfpg.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgolfdb.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgolflite.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgolfmys.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgolfsec.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgolftree.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgolfcurl.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgolfxml.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgolfarr.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgolfpcre2.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libsrvcgolf.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgolf.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgolfcli.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgolfscli.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 stub_sqlite.o -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 stub_postgres.o -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 stub_mariadb.o -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 stub_gendb.o -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 stub_srvc.o -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 stub_pcre2.o -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 stub_curl.o -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 stub_arr.o -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 stub_tree.o -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 stub_crypto.o -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 stub_before.o -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 stub_after.o -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 stub_xml.o -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 golf.vim -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 vmakefile -t $(DESTDIR)$(V_LIB)/
	if [ "$(GG_DEBIAN_BUILD)" != "1" ]; then install -D -m 0644 LICENSE -t $(DESTDIR)$(V_GG_DATADIR_GOLF)/ ; fi
	install -D -m 0644 NOTICE -t $(DESTDIR)$(V_GG_DATADIR_GOLF)/
	install -D -m 0644 README.md -t $(DESTDIR)$(V_GG_DATADIR_GOLF)/
	install -D -m 0644 CONTRIBUTING.md -t $(DESTDIR)$(V_GG_DATADIR_GOLF)/
	install -m 0755 -d $(DESTDIR)$(V_BIN)
	install -D -m 0755 v1 -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 vdiag -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 gg  -t $(DESTDIR)$(V_BIN)/
	install -D -m 0755 mgrg  -t $(DESTDIR)$(V_BIN)/
	if [[ "$(GG_DEBIAN_BUILD)" != "1" && "$(GG_FEDORA_BUILD)" != "1" && -f v1.dbg ]]; then install -m 0755 -d $(DESTDIR)$(V_LIBD) ; install -D -m 0644 v1.dbg -t $(DESTDIR)$(V_LIBD)/ ; install -D -m 0644 mgrg.dbg  -t $(DESTDIR)$(V_LIBD)/ ;  install -D -m 0644 libgolfpg.so.dbg -t $(DESTDIR)$(V_LIBD)/ ; install -D -m 0644 libgolfdb.so.dbg -t $(DESTDIR)$(V_LIBD)/ ; install -D -m 0644 libgolflite.so.dbg -t $(DESTDIR)$(V_LIBD)/ ; install -D -m 0644 libgolfmys.so.dbg -t $(DESTDIR)$(V_LIBD)/ ; install -D -m 0644 libgolfsec.so.dbg -t $(DESTDIR)$(V_LIBD)/ ; install -D -m 0644 libgolftree.so.dbg -t $(DESTDIR)$(V_LIBD)/ ; install -D -m 0644 libgolfcurl.so.dbg -t $(DESTDIR)$(V_LIBD)/ ; install -D -m 0644 libgolfxml.so.dbg -t $(DESTDIR)$(V_LIBD)/ ; install -D -m 0644 libgolfarr.so.dbg -t $(DESTDIR)$(V_LIBD)/ ; install -D -m 0644 libgolfpcre2.so.dbg -t $(DESTDIR)$(V_LIBD)/ ; install -D -m 0644 libsrvcgolf.so.dbg -t $(DESTDIR)$(V_LIBD)/ ; install -D -m 0644 libgolf.so.dbg -t $(DESTDIR)$(V_LIBD)/ ; install -D -m 0644 libgolfcli.so.dbg -t $(DESTDIR)$(V_LIBD)/ ; install -D -m 0644 libgolfscli.so.dbg -t $(DESTDIR)$(V_LIBD)/ ; fi
	if [ "$(GG_DEBIAN_BUILD)" != "1" ]; then install -m 0755 -d $(DESTDIR)$(V_MAN) ; install -D -m 0644 docs/*.2gg -t $(DESTDIR)$(V_MAN)/ ; if [ "$(NOUPDOCS)" != "1" ]; then sed -i "s/\$$VERSION/$(PACKAGE_VERSION)/g" $(DESTDIR)$(V_MAN)/*.2gg; fi ; if [ "$(NOUPDOCS)" != "1" ]; then sed -i "s/\$$DATE/$(DATE)/g" $(DESTDIR)$(V_MAN)/*.2gg; fi ; for i in $$(ls $(DESTDIR)$(V_MAN)/*.2gg); do gzip -f $$i; done ; mandb >/dev/null 2>&1 || true ; fi
	install -D -m 0755 sys -t $(DESTDIR)$(V_LIB)/
#
#
#these settings must have GG_ROOT for installed files to work properly (i.e. to reference the correct path). GG_ROOT is not the same as DESTDIR. GG_ROOT is the root directory of a local install, while DESTDIR is a 'fake root' for building.
	sed -i "s|^[ ]*export[ ]*GG_LIBRARY_PATH[ ]*=.*|export GG_LIBRARY_PATH=$(GG_ROOT)$(V_LIB)|g" $(DESTDIR)$(V_LIB)/sys
	sed -i "s|^[ ]*export[ ]*GG_LIBRARY_PATH[ ]*=.*|export GG_LIBRARY_PATH=$(GG_ROOT)$(V_LIB)|g" $(DESTDIR)$(V_BIN)/gg
	sed -i "s|^[ ]*export[ ]*GG_INCLUDE_PATH[ ]*=.*|export GG_INCLUDE_PATH=$(GG_ROOT)$(V_INC)|g" $(DESTDIR)$(V_LIB)/sys
	sed -i "s|^[ ]*export[ ]*GG_VERSION[ ]*=.*|export GG_VERSION=$(PACKAGE_VERSION)|g" $(DESTDIR)$(V_LIB)/sys
#For local installation, make sure golf.h and gcli.h have proper GG_ROOT set, so that even plain gcc building for a C client works (i.e. GG_ROOT is defined properly)
	if [ "$(GG_LOCAL)" == "1" ]; then sed -i 's|^export GG_ROOT=.*$$|export GG_ROOT="'$(GG_ROOT)'"|g' $(DESTDIR)/usr/bin/gg ; sed -i 's|^#define GG_ROOT .*$$|#define GG_ROOT "'$(GG_ROOT)'"|g' $(DESTDIR)/usr/include/golf/golf.h ; sed -i 's|^#\s*define GG_ROOT .*$$|#define GG_ROOT "'$(GG_ROOT)'"|g' $(DESTDIR)/usr/include/golf/gcli.h ; fi
#end of setting variables with GG_ROOT
#
#
	chmod -x *.so; chmod -x *.o
#This must be last, in this order, as it saves and then applies SELinux policy where applicable. 
#This runs during rpm creation or during sudo make install
#it does NOT run during rpm installation, there is post scriptlet that calls golf.sel to do that (GG_NO_SEL)
	install -D -m 0644 gg.te -t $(DESTDIR)$(V_LIB)/selinux 
	install -D -m 0644 golf.te -t $(DESTDIR)$(V_LIB)/selinux 
	install -D -m 0755 golf.sel -t $(DESTDIR)$(V_LIB)/selinux 
#this selinux.setup will be executed in rpm post section, and here (below) if this is a system with SELinux
#the text saved to selinux.setup does NOT use DESTDIR, since this script never runs in fake root, meaning if it does run, then DESTDIR is empty (which is
#sudo make install from source) or it runs during %post section when installing package. So in either case, not a fake root.
#This way we also have a script to re-run policy setup if something isn't right
	install -D -m 0755 selinux.setup -t $(DESTDIR)$(V_LIB)/selinux
#this has to be double negation because it's purpose is to prevent selinux setup in fake root, which is only from golf.spec rpmbuild where GG_NOSEL is set to 1
#also SELinux cannot be installed with local install since it requires root access to change policies. So for Fedora-like distros, local setup is for permissive only
#(or run the script below as root!)
	if [[ "$(GG_LOCAL)" != "1" && "$(GG_FAKEROOT)" != "1" && -f /etc/selinux/config ]]; then $(DESTDIR)$(V_LIB)/selinux/selinux.setup; fi

.PHONY: uninstall
uninstall:
	@if [ ! -f "$(DESTDIR)$(V_LIB)/sys" ]; then echo "Golf not installed, cannot uninstall."; exit -1; else echo "Uninstalling Golf..."; fi
	. $(DESTDIR)$(V_LIB)/sys
	rm -rf $(DESTDIR)$(V_INC)
	rm -f $(DESTDIR)$(V_BIN)/gg
	rm -f $(DESTDIR)$(V_BIN)/mgrg
	rm -f $(DESTDIR)$(V_MAN)/*.2gg ; rm -f $(DESTDIR)$(V_MAN)/*.2gg.gz ; mandb >/dev/null 2>&1 || true
	rm -rf $(DESTDIR)$(V_LIB)
	rm -rf $(DESTDIR)$(V_LIBD)

.PHONY: binary
binary:build
	@;

.PHONY: build
build: libsrvcgolf.so libgolfcli.so libgolfscli.so libgolf.so libgolfdb.so libgolfsec.so libgolfmys.so libgolflite.so libgolfpg.so libgolfcurl.so libgolfxml.so libgolfarr.so libgolftree.so libgolfpcre2.so stub_sqlite.o stub_postgres.o stub_mariadb.o stub_gendb.o stub_curl.o stub_xml.o stub_arr.o stub_tree.o stub_pcre2.o stub_srvc.o stub_crypto.o stub_after.o stub_before.o mgrg v1 selinux.setup
	@echo "Building version $(PACKAGE_VERSION)"

.PHONY: clean
clean:
	touch *.c
	touch *.h
	rm -rf debian/golf
	rm -rf debian/golf.substvars
	rm -rf debian/.debhelper
	rm -rf debian/debhelper*
	rm -rf debian/files
	rm -rf *.tar.gz
	rm -f *.o
	rm -f *.so
	rm -f *.dbg
	rm -f mgrg v1
	rm -f selinux.setup



#
# Other than GOLF preprocessor, we do NOT use any libraries at customer's site - 
# the Makefile for application (such as in hello world example) will link with
# those libraries AT customer site.
#
v1.o: v1.c golf.h
	$(CC) -c -o $@ $< $(CFLAGS) 

v1: v1.o golfmems.o chandle.o golfrtc.o hash.o  
	$(CC) -o v1 $^ $(LFLAGS) 
	$(call strip_sym)

selinux.setup:
	echo '#!/bin/bash'>selinux.setup
	if [ "$(GG_LOCAL)" == "1" ]; then SEDIR="$(DESTDIR)"; else SEDIR=""; fi ; echo "if [ -f /etc/selinux/config ]; then $$SEDIR$(V_LIB)/selinux/golf.sel '$$SEDIR$(V_LIB)/selinux' '$(V_GG_DATADIR)' '$$SEDIR' ; fi">>selinux.setup
	chmod 0755 selinux.setup

mgrg: mgrg.o 
	$(CC) -o mgrg mgrg.o $(LFLAGS) 
	$(call strip_sym)

libsrvcgolf.so: chandle.o hash.o json.o msg.o utf.o srvc_golfrt.o golfrtc.o golfmems.o 
	rm -f libsrvcgolf.so
	$(CC) -shared -o libsrvcgolf.so $^ $(LFLAGS_COMMON)
	$(call strip_sym)

libgolf.so: chandle.o hash.o json.o msg.o utf.o golfrt.o golfrtc.o golfmems.o 
	rm -f libgolf.so
	$(CC) -shared -o libgolf.so $^ $(LFLAGS_COMMON)
	$(call strip_sym)

libgolfpg.so: pg.o 
	rm -f libgolfpg.so
	$(CC) -shared -o libgolfpg.so $^ $(LFLAGS_COMMON)
	$(call strip_sym)

libgolflite.so: lite.o 
	rm -f libgolflite.so
	$(CC) -shared -o libgolflite.so $^ $(LFLAGS_COMMON)
	$(call strip_sym)

libgolfmys.so: mys.o 
	rm -f libgolfmys.so
	$(CC) -shared -o libgolfmys.so $^ $(LFLAGS_COMMON)
	$(call strip_sym)

libgolfdb.so: db.o 
	rm -f libgolfdb.so
	$(CC) -shared -o libgolfdb.so $^ $(LFLAGS_COMMON)
	$(call strip_sym)

libgolfsec.so: sec.o 
	rm -f libgolfsec.so
	$(CC) -shared -o libgolfsec.so  $^ $(LFLAGS_COMMON)
	$(call strip_sym)

libgolfxml.so: xml.o 
	rm -f libgolfxml.so
	$(CC) -shared -o libgolfxml.so $^ $(LFLAGS_COMMON)
	$(call strip_sym)

libgolfarr.so: arr_string.o  arr_number.o arr_bool.o
	rm -f libgolfarr.so
	$(CC) -shared -o libgolfarr.so $^ $(LFLAGS_COMMON)
	$(call strip_sym)

libgolfcurl.so: curl.o 
	rm -f libgolfcurl.so
	$(CC) -shared -o libgolfcurl.so $^ $(LFLAGS_COMMON)
	$(call strip_sym)

libgolftree.so: tree.o 
	rm -f libgolftree.so
	$(CC) -shared -o libgolftree.so $^ $(LFLAGS_COMMON)
	$(call strip_sym)

libgolfpcre2.so: pcre2.o 
	rm -f libgolfpcre2.so
	$(CC) -shared -o libgolfpcre2.so $^ $(LFLAGS_COMMON)
	$(call strip_sym)

utf.o: utf.c golf.h
	$(CC) -c -o $@ $< $(CFLAGS) 

hash.o: hash.c golf.h
	$(CC) -c -o $@ $< $(CFLAGS) 

json.o: json.c golf.h
	$(CC) -c -o $@ $< $(CFLAGS) 

msg.o: msg.c golf.h
	$(CC) -c -o $@ $< $(CFLAGS) 

tree.o: tree.c golf.h
	$(CC) -c -o $@ $< $(CFLAGS) 

chandle.o: chandle.c golf.h
	$(CC) -c -o $@ $< $(CFLAGS) 

pg.o: pg.c golf.h
	$(CC) -c -o $@ $< $(CFLAGS) 

db.o: db.c golf.h
	$(CC) -c -o $@ $< $(CFLAGS) 

lite.o: lite.c golf.h
	$(CC) -c -o $@ $< $(CFLAGS)

mys.o: mys.c golf.h
	$(CC) -c -o $@ $< $(CFLAGS)

sec.o: sec.c golf.h
	$(CC) -c -o $@ $< $(CFLAGS) 

stub_after.o: stub_after.c golf.h
	$(CC) -c -o $@ $< $(CFLAGS)

stub_before.o: stub_before.c golf.h
	$(CC) -c -o $@ $< $(CFLAGS)

stub_crypto.o: stub.c golf.h
	$(CC) -c -o $@ -DGG_CRYPTO $< $(CFLAGS) 

stub_xml.o: stub.c golf.h
	$(CC) -c -o $@ -DGG_XML $< $(CFLAGS) 

stub_curl.o: stub.c golf.h
	$(CC) -c -o $@ -DGG_CURL $< $(CFLAGS)

stub_arr.o: stub.c golf.h
	$(CC) -c -o $@ -DGG_ARR $< $(CFLAGS)

stub_tree.o: stub.c golf.h
	$(CC) -c -o $@ -DGG_TREE $< $(CFLAGS) 

stub_pcre2.o: stub.c golf.h
	$(CC) -c -o $@ -DGG_PCRE2 $< $(CFLAGS) 

stub_srvc.o: stub.c golf.h
	$(CC) -c -o $@ -DGG_SERVICE $< $(CFLAGS) 

stub_sqlite.o: stub.c golf.h
	$(CC) -c -o $@ -DGG_STUB_SQLITE $< $(CFLAGS) 

stub_postgres.o: stub.c golf.h
	$(CC) -c -o $@ -DGG_STUB_POSTGRES $< $(CFLAGS) 

stub_mariadb.o: stub.c golf.h
	$(CC) -c -o $@ -DGG_STUB_MARIADB $< $(CFLAGS) 

stub_gendb.o: stub.c golf.h
	$(CC) -c -o $@ -DGG_STUB_GENDB $< $(CFLAGS) 

curl.o: curl.c golf.h
	$(CC) -c -o $@ $< $(CFLAGS) 

xml.o: xml.c golf.h
	$(CC) -c -o $@ $< $(CFLAGS) 

arr_string.o: arr.c golf.h
	$(CC) -c -o $@ $< $(CFLAGS) -DGG_ARR_STRING

arr_number.o: arr.c golf.h
	$(CC) -c -o $@ $< $(CFLAGS) -DGG_ARR_NUMBER

arr_bool.o: arr.c golf.h
	$(CC) -c -o $@ $< $(CFLAGS) -DGG_ARR_BOOL

pcre2.o: pcre2.c golf.h
	NEWPCRE2=$$(./sys greater_than_eq "$(PCRE2_VER)" "10.37"); if [ "$$NEWPCRE2" == "0" ]; then GLIBC_REGEX="-DGG_C_GLIBC_REGEX"; fi ; $(CC) -c -o $@ $< $$GLIBC_REGEX $(CFLAGS) 


golfrtc.o: golfrtc.c golf.h
	$(CC) -c -o $@ $< $(CFLAGS)

srvc_golfrt.o: golfrt.c golf.h
	$(CC) -c -o $@ $< $(CFLAGS)  

golfrt.o: golfrt.c golf.h
	$(CC) -c -DGG_COMMAND -o $@ $< $(CFLAGS)  

golfmems.o: golfmem.c golf.h
	$(CC) -c -o $@ $< $(CFLAGS) 

mgrg.o: mgrg.c 
	$(CC) -c -o $@ $< $(CFLAGS) 

libgolfcli.so: gcli.c gcli.h golf.h
	rm -f libgolfcli.so
	$(CC) -shared -o libgolfcli.so $^ $(CFLAGS)
	$(call strip_sym)

libgolfscli.so: gcli.c gcli.h golf.h
	rm -f libgolfscli.so
	$(CC) -shared -o libgolfscli.so $^ $(CFLAGS) -DGG_GOLFSRV
	$(call strip_sym)
