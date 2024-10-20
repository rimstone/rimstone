#SPDX-License-Identifier: Apache-2.0
#Copyright 2019 Gliim LLC.  
#Licensed under Apache License v2. See LICENSE file.
#On the web http://gliimly.github.io/ - this file is part of Gliimly framework.

#makefile for Gliimly

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
    V_LIB=$(shell rpm -E '%{_libdir}')/gliim
else
    V_LIB=/usr/lib/gliim
endif

ifeq ($(strip $(SYSTEMTYPE)),fedora)
    V_INC=$(shell rpm -E '%{_includedir}')/gliim
else
    V_INC=/usr/include/gliim
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
    V_GG_DOCS=$(shell rpm -E '%{_datadir}')/gliim
else
    V_GG_DATADIR=/usr/share
    V_GG_DOCS=/usr/share/gliim
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
CFLAGS=-std=gnu89 -Werror -Wall -Wextra -Wuninitialized -Wmissing-declarations -Wformat -Wno-format-zero-length -fsigned-char -fpic $(GG_MARIA_INCLUDE) $(GG_POSTGRES_INCLUDE) $(GG_SERVICE_INCLUDE) -DGG_OSNAME_ID=$(OSNAMEID) -DGG_OSNAME="\"$(OSNAME)\"" -DGG_OSVERSION="\"$(OSVERSION)\"" -DGG_PKGVERSION="\"$(PACKAGE_VERSION)\"" $(OPTIM_COMP) $(ASAN) -fmax-errors=5

#linker flags include mariadb (LGPL), crypto (OpenSSL, permissive license). This is for building object code that's part 
#this is for installation at customer's site where we link GLIIMLY with mariadb (LGPL), crypto (OpenSSL)
LDFLAGS=-Wl,-rpath=$(DESTDIR)$(V_LIB) -L$(DESTDIR)$(V_LIB) $(OPTIM_LINK) $(ASAN)

#Libraries and executables must be 0755 or the packager (RPM) will say they are not satisfied
.PHONY: install
install:
	install -m 0755 -d $(DESTDIR)/var/lib/gg/bld
	install -m 0755 -d $(DESTDIR)$(V_INC)
	install -D -m 0644 gliim.h -t $(DESTDIR)$(V_INC)/
	install -D -m 0644 gcli.h -t $(DESTDIR)$(V_INC)/
	install -m 0755 -d $(DESTDIR)$(V_LIB)
	install -D -m 0755 libgliimpg.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgliimdb.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgliimlylite.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgliimmys.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgliimsec.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgliimtree.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgliimcurl.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgliimpcre2.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgliimpcre2glibc.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libsrvcgliim.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgliim.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgliimcli.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0755 libgliimscli.so -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 stub_sqlite.o -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 stub_postgres.o -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 stub_mariadb.o -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 stub_gendb.o -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 stub_srvc.o -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 stub_pcre2.o -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 stub_curl.o -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 stub_tree.o -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 stub_crypto.o -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 stub_before.o -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 stub_after.o -t $(DESTDIR)$(V_LIB)/
	install -D -m 0644 gliim.vim -t $(DESTDIR)$(V_LIB)/
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
	install -D -m 0644 docs/gliimdoc.html -t $(DESTDIR)$(V_GG_DOCS)/
	echo -n "$(PCRE2_VER)" > pcre2_version; install -D -m 0755 pcre2_version -t $(DESTDIR)$(V_LIB)/
	echo -n "$(PCRE2_LIBS)" > pcre2_libs; install -D -m 0755 pcre2_libs -t $(DESTDIR)$(V_LIB)/
#This must be last, in this order, as it saves and then applies SELinux policy where applicable. 
#This runs during rpm creation or during sudo make install
#it does NOT run during rpm installation, there is post scriptlet that calls gliim.sel to do that (GG_NO_SEL)
	if [[ "$(SYSTEMTYPE)" == "fedora" && "$(SYSTEMID)" != "opensuse" ]]; then install -D -m 0644 gg.te -t $(DESTDIR)$(V_LIB)/selinux ; install -D -m 0644 gliim.te -t $(DESTDIR)$(V_LIB)/selinux ; install -D -m 0644 gg.fc -t $(DESTDIR)$(V_LIB)/selinux ; install -D -m 0755 gliim.sel -t $(DESTDIR)$(V_LIB)/selinux ; if [ "$(GG_NO_SEL)" != "1" ]; then ./gliim.sel "$(DESTDIR)$(V_LIB)/selinux" "$(DESTDIR)$(V_GG_DATADIR)" "$(DESTDIR)$(V_BIN)"; fi ; fi

.PHONY: uninstall
uninstall:
	@if [ ! -f "$(DESTDIR)$(V_LIB)/sys" ]; then echo "Gliimly not installed, cannot uninstall."; exit -1; else echo "Uninstalling Gliimly..."; fi
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
build: libsrvcgliim.so libgliimcli.so libgliimscli.so libgliim.so libgliimdb.so libgliimsec.so libgliimmys.so libgliimlylite.so libgliimpg.so libgliimcurl.so libgliimtree.so libgliimpcre2.so libgliimpcre2glibc.so v1.o stub_sqlite.o stub_postgres.o stub_mariadb.o stub_gendb.o stub_curl.o stub_tree.o stub_pcre2.o stub_srvc.o stub_crypto.o stub_after.o stub_before.o mgrg 
	@echo "Building version $(PACKAGE_VERSION)"
	$(CC) -o v1 v1.o chandle.o gliimrtc.o gliimmemc.o hash.o $(LDFLAGS) 

.PHONY: clean
clean:
	touch *.c
	touch *.h
	rm -rf debian/gliim
	rm -rf *.tar.gz
	rm -f pcre2_version pcre2_libs



#
# Other than GLIIMLY preprocessor, we do NOT use any libraries at customer's site - 
# the Makefile for application (such as in hello world example) will link with
# those libraries AT customer site.
#
v1.o: v1.c gliimmemc.o
	$(CC) -c -o $@ $< $(CFLAGS) 

mgrg: mgrg.o 
	$(CC) -o mgrg mgrg.o $(LDFLAGS)

libsrvcgliim.so: chandle.o hash.o json.o msg.o utf8.o srvc_gliimrt.o gliimrtc.o gliimmems.o 
	rm -f libsrvcgliim.so
	$(CC) -shared -o libsrvcgliim.so $^ 
	if [ "$(DEBUGINFO)" != "1" ]; then strip --strip-unneeded libsrvcgliim.so ; fi

libgliim.so: chandle.o hash.o json.o msg.o utf8.o gliimrt.o gliimrtc.o gliimmems.o 
	rm -f libgliim.so
	$(CC) -shared -o libgliim.so $^ 
	if [ "$(DEBUGINFO)" != "1" ]; then strip --strip-unneeded libgliim.so ; fi

libgliimpg.so: pg.o 
	rm -f libgliimpg.so
	$(CC) -shared -o libgliimpg.so $^ 
	if [ "$(DEBUGINFO)" != "1" ]; then strip --strip-unneeded libgliimpg.so ; fi

libgliimlylite.so: lite.o 
	rm -f libgliimlylite.so
	$(CC) -shared -o libgliimlylite.so $^ 
	if [ "$(DEBUGINFO)" != "1" ]; then strip --strip-unneeded libgliimlylite.so ; fi

libgliimmys.so: mys.o 
	rm -f libgliimmys.so
	$(CC) -shared -o libgliimmys.so $^ 
	if [ "$(DEBUGINFO)" != "1" ]; then strip --strip-unneeded libgliimmys.so ; fi

libgliimdb.so: db.o 
	rm -f libgliimdb.so
	$(CC) -shared -o libgliimdb.so $^ 
	if [ "$(DEBUGINFO)" != "1" ]; then strip --strip-unneeded libgliimdb.so ; fi

libgliimsec.so: sec.o 
	rm -f libgliimsec.so
	$(CC) -shared -o libgliimsec.so  $^ 
	if [ "$(DEBUGINFO)" != "1" ]; then strip --strip-unneeded libgliimsec.so ; fi

libgliimcurl.so: curl.o 
	rm -f libgliimcurl.so
	$(CC) -shared -o libgliimcurl.so $^ 
	if [ "$(DEBUGINFO)" != "1" ]; then strip --strip-unneeded libgliimcurl.so ; fi

libgliimtree.so: tree.o 
	rm -f libgliimtree.so
	$(CC) -shared -o libgliimtree.so $^ 
	if [ "$(DEBUGINFO)" != "1" ]; then strip --strip-unneeded libgliimtree.so ; fi

libgliimpcre2.so: pcre2.o 
	rm -f libgliimpcre2.so
	$(CC) -shared -o libgliimpcre2.so $^ 
	if [ "$(DEBUGINFO)" != "1" ]; then strip --strip-unneeded libgliimpcre2.so ; fi

libgliimpcre2glibc.so: pcre2glibc.o 
	rm -f libgliimpcre2glibc.so
	$(CC) -shared -o libgliimpcre2glibc.so $^ 
	if [ "$(DEBUGINFO)" != "1" ]; then strip --strip-unneeded libgliimpcre2glibc.so ; fi

utf8.o: utf8.c gliim.h
	$(CC) -c -o $@ $< $(CFLAGS) 

hash.o: hash.c gliim.h
	$(CC) -c -o $@ $< $(CFLAGS) 

json.o: json.c gliim.h
	$(CC) -c -o $@ $< $(CFLAGS) 

msg.o: msg.c gliim.h
	$(CC) -c -o $@ $< $(CFLAGS) 

tree.o: tree.c gliim.h
	$(CC) -c -o $@ $< $(CFLAGS) 

chandle.o: chandle.c gliim.h
	$(CC) -c -o $@ $< $(CFLAGS) 

pg.o: pg.c gliim.h
	$(CC) -c -o $@ $< $(CFLAGS) 

db.o: db.c gliim.h
	$(CC) -c -o $@ $< $(CFLAGS) 

lite.o: lite.c gliim.h
	$(CC) -c -o $@ $< $(CFLAGS)

mys.o: mys.c gliim.h
	$(CC) -c -o $@ $< $(CFLAGS)

sec.o: sec.c gliim.h
	$(CC) -c -o $@ $< $(CFLAGS) 

stub_after.o: stub_after.c gliim.h
	$(CC) -c -o $@ $< $(CFLAGS)

stub_before.o: stub_before.c gliim.h
	$(CC) -c -o $@ $< $(CFLAGS)

stub_crypto.o: stub.c gliim.h
	$(CC) -c -o $@ -DGG_CRYPTO $< $(CFLAGS) 

stub_curl.o: stub.c gliim.h
	$(CC) -c -o $@ -DGG_CURL $< $(CFLAGS)

stub_tree.o: stub.c gliim.h
	$(CC) -c -o $@ -DGG_TREE $< $(CFLAGS) 

stub_pcre2.o: stub.c gliim.h
	$(CC) -c -o $@ -DGG_PCRE2 $< $(CFLAGS) 

stub_srvc.o: stub.c gliim.h
	$(CC) -c -o $@ -DGG_SERVICE $< $(CFLAGS) 

stub_sqlite.o: stub.c gliim.h
	$(CC) -c -o $@ -DGG_STUB_SQLITE $< $(CFLAGS) 

stub_postgres.o: stub.c gliim.h
	$(CC) -c -o $@ -DGG_STUB_POSTGRES $< $(CFLAGS) 

stub_mariadb.o: stub.c gliim.h
	$(CC) -c -o $@ -DGG_STUB_MARIADB $< $(CFLAGS) 

stub_gendb.o: stub.c gliim.h
	$(CC) -c -o $@ -DGG_STUB_GENDB $< $(CFLAGS) 

curl.o: curl.c gliim.h
	$(CC) -c -o $@ $< $(CFLAGS) 

pcre2.o: pcre2.c gliim.h
	NEWPCRE2=$$(./sys greater_than_eq "$(PCRE2_VER)" "10.37"); if [ "$$NEWPCRE2" == "0" ]; then FORCE_POSIXREGEX="-DGG_C_POSIXREGEX"; else FORCE_POSIXREGEX="-DPCRE2_LIB_DL=\"$(PCRE2_LIB_DL)\""; fi ; $(CC) -c -o $@ $< $$FORCE_POSIXREGEX $(CFLAGS) 

pcre2glibc.o: pcre2.c gliim.h
	$(CC) -c -o $@ $< -DGG_C_POSIXREGEX $(CFLAGS) 

gliimrtc.o: gliimrtc.c gliim.h
	$(CC) -c -o $@ $< $(CFLAGS)

srvc_gliimrt.o: gliimrt.c gliim.h
	$(CC) -c -o $@ $< $(CFLAGS)  

gliimrt.o: gliimrt.c gliim.h
	$(CC) -c -DGG_COMMAND -o $@ $< $(CFLAGS)  

gliimmemc.o: gliimmem.c gliim.h
	$(CC) -c -o $@ $< $(CFLAGS) -DGG_MEM_CLIENT

gliimmems.o: gliimmem.c gliim.h
	$(CC) -c -o $@ $< $(CFLAGS) 

mgrg.o: mgrg.c 
	$(CC) -c -o $@ $< $(CFLAGS) 

libgliimcli.so: gcli.c gcli.h
	rm -f libgliimcli.so
	$(CC) -shared -o libgliimcli.so $^ $(CFLAGS)
	if [ "$(DEBUGINFO)" != "1" ]; then strip --strip-unneeded libgliimcli.so ; fi

libgliimscli.so: gcli.c gcli.h
	rm -f libgliimscli.so
	$(CC) -shared -o libgliimscli.so $^ $(CFLAGS) -DGG_GLIIMLYSRV
	if [ "$(DEBUGINFO)" != "1" ]; then strip --strip-unneeded libgliimscli.so ; fi



