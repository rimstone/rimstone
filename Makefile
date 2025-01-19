#SPDX-License-Identifier: Apache-2.0
#Copyright 2019 Gliim LLC.  
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
PCRE2_LIB_DL=$(shell echo -n "$(PCRE2_LIBS)" |sed -n 's/-l\([^[:space:]]*\).*$$/\1/gp')
#these must be the same (GG_PLATFORM_ID,GG_PLATFORM_VERSION) used in sys
OSNAME=$(shell . ./sys; echo -n $${GG_PLATFORM_ID})
OSVERSION=$(shell . ./sys; echo -n $${GG_PLATFORM_VERSION})
OSNAMEID=$(shell . ./sys; echo -n $${GG_OSNAME_ID})
SYSTEMTYPE=$(shell ./sys showtype)
SYSTEMID=$(shell ./sys showid)
PGCONF=$(shell ./sys pgconf)
ANNOBIN=$(shell ./sys annobin)
DATE=$(shell date +"%b-%d-%Y")
GG_OS_CLOSE_2=$(shell grep OS_Close /usr/include/fcgios.h|grep shutdown|wc -l)

#these 2 must match application name from config file from each application

CC=gcc

#get build version and release
PACKAGE_VERSION=$(shell . .version; echo $${PACKAGE_VERSION})


ifeq ($(strip $(PACKAGE_VERSION)),)
PACKAGE_VERSION=2
endif

ifeq ($(strip $(SYSTEMTYPE)),fedora)
    V_LIB=$(shell rpm -E '%{_libdir}')/golf
else
    V_LIB=/usr/lib/golf
endif

ifeq ($(strip $(SYSTEMTYPE)),fedora)
    V_INC=$(shell rpm -E '%{_includedir}')/golf
else
    V_INC=/usr/include/golf
endif

ifeq ($(strip $(SYSTEMTYPE)),fedora)
    V_BIN=$(shell rpm -E '%{_bindir}')
else
    V_BIN=/usr/bin
endif

ifeq ($(strip $(SYSTEMTYPE)),fedora)
    V_MAN=$(shell rpm -E '%{_mandir}')/man2
else
    V_MAN=/usr/share/man/man2
endif
#see if man pages exist (or if not, need reindex)
MANEXIST=$(shell if [ -d "$(V_MAN)" ]; then echo "1"; else echo "0"; fi)


ifeq ($(strip $(SYSTEMTYPE)),fedora)
    V_GG_DATADIR=$(shell rpm -E '%{_datadir}')
    V_GG_DOCS=$(shell rpm -E '%{_datadir}')/golf
else
    V_GG_DATADIR=/usr/share
    V_GG_DOCS=/usr/share/golf
endif

ifeq ($(strip $(SYSTEMID)),opensuse)
    GG_SERVICE_INCLUDE=-I /usr/include/fastcgi
else
    GG_SERVICE_INCLUDE=
endif

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
OPTIM_COMP_DEBUG=-g3 -DDEBUG -rdynamic
OPTIM_COMP_PROD=-O3 
OPTIM_LINK_PROD=
OPTIM_LINK_DEBUG=-rdynamic
ifeq ($(DEBUGINFO),1)
OPTIM_COMP=$(OPTIM_COMP_DEBUG)
OPTIM_LINK=$(OPTIM_LINK_DEBUG)
else
OPTIM_COMP=$(OPTIM_COMP_PROD)
OPTIM_LINK=$(OPTIM_LINK_PROD)
endif
ifeq ($(ASAN),1)
ASAN=-fsanitize=address -fsanitize-recover=address
else
ASAN=
endif

#C flags are as strict as we can do, in order to discover as many bugs as early on
CFLAGS=-std=gnu99 -Werror -Wall -Wextra -Wuninitialized -Wmissing-declarations -Wformat -Wno-format-zero-length -funsigned-char -fpic $(GG_MARIA_INCLUDE) $(GG_POSTGRES_INCLUDE) $(GG_SERVICE_INCLUDE) $(GG_LIBXML2_INCLUDE) -DGG_OSNAME_ID=$(OSNAMEID) -DGG_OSNAME="\"$(OSNAME)\"" -DGG_OSVERSION="\"$(OSVERSION)\"" -DGG_PKGVERSION="\"$(PACKAGE_VERSION)\"" $(OPTIM_COMP) $(ASAN) -fmax-errors=5

#linker flags include mariadb (LGPL), crypto (OpenSSL, permissive license). This is for building object code that's part 
#this is for installation at customer's site where we link GOLF with mariadb (LGPL), crypto (OpenSSL)
LDFLAGS=-Wl,-rpath=$(DESTDIR)$(V_LIB) -L$(DESTDIR)$(V_LIB) $(OPTIM_LINK) $(ASAN)

#Libraries and executables must be 0755 or the packager (RPM) will say they are not satisfied
.PHONY: install
install:
	install -m 0755 -d $(DESTDIR)/var/lib/gg/bld
	install -m 0755 -d $(DESTDIR)$(V_INC)
	install -D -m 0644 golf.h -t $(DESTDIR)$(V_INC)/
	install -D -m 0644 gcli.h -t $(DESTDIR)$(V_INC)/
	install -m 0755 -d $(DESTDIR)$(V_LIB)
	install -D -m 0755 libgolfpg.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgolfdb.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgolflite.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgolfmys.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgolfsec.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgolftree.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgolfcurl.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgolfxml.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgolfpcre2.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgolfpcre2glibc.so -t $(DESTDIR)$(V_LIB)/
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
	install -D -m 0644 stub_xml.o -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 stub_tree.o -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 stub_crypto.o -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 stub_before.o -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 stub_after.o -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 golf.vim -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 vmakefile -t $(DESTDIR)$(V_LIB)/
	install -m 0755 -d $(DESTDIR)$(V_BIN)
	install -D -m 0755 v1 -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 vdiag -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 gg  -t $(DESTDIR)$(V_BIN)/
	install -D -m 0755 mgrg  -t $(DESTDIR)$(V_BIN)/
	install -m 0755 -d $(DESTDIR)$(V_MAN)
	install -D -m 0644 docs/*.2gg -t $(DESTDIR)$(V_MAN)/
	install -D -m 0755 sys -t $(DESTDIR)$(V_LIB)/
	sed -i "s|^[ ]*export[ ]*GG_LIBRARY_PATH[ ]*=.*|export GG_LIBRARY_PATH=$(V_LIB)|g" $(DESTDIR)$(V_LIB)/sys
	sed -i "s|^[ ]*export[ ]*GG_LIBRARY_PATH[ ]*=.*|export GG_LIBRARY_PATH=$(V_LIB)|g" $(DESTDIR)$(V_BIN)/gg
	sed -i "s|^[ ]*export[ ]*GG_INCLUDE_PATH[ ]*=.*|export GG_INCLUDE_PATH=$(V_INC)|g" $(DESTDIR)$(V_LIB)/sys
	sed -i "s|^[ ]*export[ ]*GG_VERSION[ ]*=.*|export GG_VERSION=$(PACKAGE_VERSION)|g" $(DESTDIR)$(V_LIB)/sys
	if [ "$(NOUPDOCS)" != "1" ]; then sed -i "s/\$$VERSION/$(PACKAGE_VERSION)/g" $(DESTDIR)$(V_MAN)/*.2gg; fi
	if [ "$(NOUPDOCS)" != "1" ]; then sed -i "s/\$$DATE/$(DATE)/g" $(DESTDIR)$(V_MAN)/*.2gg; fi
	for i in $$(ls $(DESTDIR)$(V_MAN)/*.2gg); do gzip -f $$i; done
	install -m 0755 -d $(DESTDIR)$(V_GG_DOCS)
	install -D -m 0644 docs/golfdoc.html -t $(DESTDIR)$(V_GG_DOCS)/
	echo -n "$(PCRE2_VER)" > pcre2_version; install -D -m 0755 pcre2_version -t $(DESTDIR)$(V_LIB)/
	echo -n "$(PCRE2_LIBS)" > pcre2_libs; install -D -m 0755 pcre2_libs -t $(DESTDIR)$(V_LIB)/
#This must be last, in this order, as it saves and then applies SELinux policy where applicable. 
#This runs during rpm creation or during sudo make install
#it does NOT run during rpm installation, there is post scriptlet that calls golf.sel to do that (GG_NO_SEL)
	if [[ "$(SYSTEMTYPE)" == "fedora" && "$(SYSTEMID)" != "opensuse" ]]; then install -D -m 0644 gg.te -t $(DESTDIR)$(V_LIB)/selinux ; install -D -m 0644 golf.te -t $(DESTDIR)$(V_LIB)/selinux ; install -D -m 0644 gg.fc -t $(DESTDIR)$(V_LIB)/selinux ; install -D -m 0755 golf.sel -t $(DESTDIR)$(V_LIB)/selinux ; if [ "$(GG_NO_SEL)" != "1" ]; then ./golf.sel "$(DESTDIR)$(V_LIB)/selinux" "$(DESTDIR)$(V_GG_DATADIR)" "$(DESTDIR)$(V_BIN)"; fi ; fi

.PHONY: uninstall
uninstall:
	@if [ ! -f "$(DESTDIR)$(V_LIB)/sys" ]; then echo "Golf not installed, cannot uninstall."; exit -1; else echo "Uninstalling Golf..."; fi
	. $(DESTDIR)$(V_LIB)/sys
	rm -rf $(DESTDIR)$(V_INC)
	rm -f $(DESTDIR)$(V_BIN)/gg
	rm -f $(DESTDIR)$(V_BIN)/mgrg
	rm -f $(DESTDIR)$(V_MAN)/*.2gg
	rm -rf $(DESTDIR)$(V_GG_DOCS)
	rm -rf $(DESTDIR)$(V_LIB)

.PHONY: binary
binary:build
	@;

.PHONY: build
build: libsrvcgolf.so libgolfcli.so libgolfscli.so libgolf.so libgolfdb.so libgolfsec.so libgolfmys.so libgolflite.so libgolfpg.so libgolfcurl.so libgolfxml.so libgolftree.so libgolfpcre2.so libgolfpcre2glibc.so v1.o stub_sqlite.o stub_postgres.o stub_mariadb.o stub_gendb.o stub_curl.o stub_xml.o stub_tree.o stub_pcre2.o stub_srvc.o stub_crypto.o stub_after.o stub_before.o mgrg 
	@echo "Building version $(PACKAGE_VERSION)"
	$(CC) -o v1 v1.o chandle.o golfrtc.o golfmemc.o hash.o $(LDFLAGS) 

.PHONY: clean
clean:
	touch *.c
	touch *.h
	rm -rf debian/golf
	rm -rf *.tar.gz
	rm -f pcre2_version pcre2_libs



#
# Other than GOLF preprocessor, we do NOT use any libraries at customer's site - 
# the Makefile for application (such as in hello world example) will link with
# those libraries AT customer site.
#
v1.o: v1.c golfmemc.o
	$(CC) -c -o $@ $< $(CFLAGS) 

mgrg: mgrg.o 
	$(CC) -o mgrg mgrg.o $(LDFLAGS)

libsrvcgolf.so: chandle.o hash.o json.o msg.o utf8.o srvc_golfrt.o golfrtc.o golfmems.o 
	rm -f libsrvcgolf.so
	$(CC) -shared -o libsrvcgolf.so $^ 
	if [ "$(DEBUGINFO)" != "1" ]; then strip --strip-unneeded libsrvcgolf.so ; fi

libgolf.so: chandle.o hash.o json.o msg.o utf8.o golfrt.o golfrtc.o golfmems.o 
	rm -f libgolf.so
	$(CC) -shared -o libgolf.so $^ 
	if [ "$(DEBUGINFO)" != "1" ]; then strip --strip-unneeded libgolf.so ; fi

libgolfpg.so: pg.o 
	rm -f libgolfpg.so
	$(CC) -shared -o libgolfpg.so $^ 
	if [ "$(DEBUGINFO)" != "1" ]; then strip --strip-unneeded libgolfpg.so ; fi

libgolflite.so: lite.o 
	rm -f libgolflite.so
	$(CC) -shared -o libgolflite.so $^ 
	if [ "$(DEBUGINFO)" != "1" ]; then strip --strip-unneeded libgolflite.so ; fi

libgolfmys.so: mys.o 
	rm -f libgolfmys.so
	$(CC) -shared -o libgolfmys.so $^ 
	if [ "$(DEBUGINFO)" != "1" ]; then strip --strip-unneeded libgolfmys.so ; fi

libgolfdb.so: db.o 
	rm -f libgolfdb.so
	$(CC) -shared -o libgolfdb.so $^ 
	if [ "$(DEBUGINFO)" != "1" ]; then strip --strip-unneeded libgolfdb.so ; fi

libgolfsec.so: sec.o 
	rm -f libgolfsec.so
	$(CC) -shared -o libgolfsec.so  $^ 
	if [ "$(DEBUGINFO)" != "1" ]; then strip --strip-unneeded libgolfsec.so ; fi

libgolfxml.so: xml.o 
	rm -f libgolfxml.so
	$(CC) -shared -o libgolfxml.so $^ 
	if [ "$(DEBUGINFO)" != "1" ]; then strip --strip-unneeded libgolfxml.so ; fi

libgolfcurl.so: curl.o 
	rm -f libgolfcurl.so
	$(CC) -shared -o libgolfcurl.so $^ 
	if [ "$(DEBUGINFO)" != "1" ]; then strip --strip-unneeded libgolfcurl.so ; fi

libgolftree.so: tree.o 
	rm -f libgolftree.so
	$(CC) -shared -o libgolftree.so $^ 
	if [ "$(DEBUGINFO)" != "1" ]; then strip --strip-unneeded libgolftree.so ; fi

libgolfpcre2.so: pcre2.o 
	rm -f libgolfpcre2.so
	$(CC) -shared -o libgolfpcre2.so $^ 
	if [ "$(DEBUGINFO)" != "1" ]; then strip --strip-unneeded libgolfpcre2.so ; fi

libgolfpcre2glibc.so: pcre2glibc.o 
	rm -f libgolfpcre2glibc.so
	$(CC) -shared -o libgolfpcre2glibc.so $^ 
	if [ "$(DEBUGINFO)" != "1" ]; then strip --strip-unneeded libgolfpcre2glibc.so ; fi

utf8.o: utf8.c golf.h
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

stub_curl.o: stub.c golf.h
	$(CC) -c -o $@ -DGG_CURL $< $(CFLAGS)

stub_xml.o: stub.c golf.h
	$(CC) -c -o $@ -DGG_XML $< $(CFLAGS)

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

pcre2.o: pcre2.c golf.h
	NEWPCRE2=$$(./sys greater_than_eq "$(PCRE2_VER)" "10.37"); if [ "$$NEWPCRE2" == "0" ]; then FORCE_POSIXREGEX="-DGG_C_POSIXREGEX"; else FORCE_POSIXREGEX="-DPCRE2_LIB_DL=\"$(PCRE2_LIB_DL)\""; fi ; $(CC) -c -o $@ $< $$FORCE_POSIXREGEX $(CFLAGS) 

pcre2glibc.o: pcre2.c golf.h
	$(CC) -c -o $@ $< -DGG_C_POSIXREGEX $(CFLAGS) 

golfrtc.o: golfrtc.c golf.h
	$(CC) -c -o $@ $< $(CFLAGS)

srvc_golfrt.o: golfrt.c golf.h
	$(CC) -c -o $@ $< $(CFLAGS)  

golfrt.o: golfrt.c golf.h
	$(CC) -c -DGG_COMMAND -o $@ $< $(CFLAGS)  

golfmemc.o: golfmem.c golf.h
	$(CC) -c -o $@ $< $(CFLAGS) -DGG_MEM_CLIENT

golfmems.o: golfmem.c golf.h
	$(CC) -c -o $@ $< $(CFLAGS) 

mgrg.o: mgrg.c 
	$(CC) -c -o $@ $< $(CFLAGS) 

libgolfcli.so: gcli.c gcli.h
	rm -f libgolfcli.so
	$(CC) -shared -o libgolfcli.so $^ $(CFLAGS)
	if [ "$(DEBUGINFO)" != "1" ]; then strip --strip-unneeded libgolfcli.so ; fi

libgolfscli.so: gcli.c gcli.h
	rm -f libgolfscli.so
	$(CC) -shared -o libgolfscli.so $^ $(CFLAGS) -DGG_GOLFSRV
	if [ "$(DEBUGINFO)" != "1" ]; then strip --strip-unneeded libgolfscli.so ; fi



