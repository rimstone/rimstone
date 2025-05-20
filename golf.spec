#SPDX-License-Identifier: Apache-2.0
#Copyright 2018-2025 Gliim LLC.
#Licensed under Apache License v2. See LICENSE file.
#On the web http://golf-lang.com/ - this file is part of Golf framework.


Name:   golf
Version:    600.3.25
Release:    1%{?dist}
Summary:    Language and server for web services and back-end solutions
Vendor:     Golf Team
Group:      Development/Tools
URL:        https://golf-lang.com/
License:    Apache-2.0
Source0: https://github.com/golf-lang/%{name}/archive/%{version}/%{name}-%{version}.tar.gz 

#Make sure to install EPEL (currently just for fcgi/fcgi_devel): sudo dnf install epel-release

#NOTE: the package includes devel packages and .h files since this is a development tool 
#Meaning there is no package and "development" package. It is *both* in one by nature.

%define all_requires make gcc openssl-devel libcurl-devel pcre2-devel libxml2-devel 

#python utils for selinux, only for rhel
%if 0%{?rhel} 
%define rhel_sel policycoreutils-python-utils
%endif

#requirements:
#sudo dnf -y install rpmdevtools dnf-utils
#sudo dnf builddep -y golf.spec

#OPENSUSE:sudo zypper -n install rpmdevtools yum-utils 
#OPENSUSE:sudo yum-builddep -y golf.spec (bug with rpmutils missing, however)
#OPENSUSE has no default policy: no SELINUX
#OPENSUSE:FastCGI is for cgi-fcgi utility
#Note: is_opensuse does seem to work on Tumbleweed, but not on Leap (where sle_version works)
%if 0%{?sle_version} ||  0%{?is_opensuse}
%define suse_requires %all_requires libmariadb-devel gpg sshpass FastCGI FastCGI-devel postgresql-devel sqlite3-devel man-pages
BuildRequires: %suse_requires 
Requires: %suse_requires 

%else
#MAGEIA:sudo dnf -y install rpmdevtools dnf-utils
#MAGEIA:sudo dnf builddep -y mageia.spec
#MAGEIA:fcgi is for cgi-fcgi utility
%if 0%{?mgaversion} >= 8
#SELINUX:policycoreutils policycoreutils-devel libselinux-utils
%define mga_requires %all_requires mariadb-devel libfcgi-devel postgresql-devel policycoreutils policycoreutils-devel libselinux-utils sqlite3-devel man-db
BuildRequires: %mga_requires 
Requires: %mga_requires 

%else
#SELINUX:policycoreutils policycoreutils-devel libselinux-utils
#fcgi is for cgi-fcgi utility; sometimes it's packaged within fcgi-devel and sometimes not.
%define rh_requires %all_requires mariadb-connector-c-devel fcgi-devel libpq-devel policycoreutils policycoreutils-devel libselinux-utils sqlite-devel man-db
BuildRequires: %rh_requires %{?rhel_sel}
Requires: %rh_requires %{?rhel_sel}
%endif
%endif


BuildRoot: %{_tmppath}/%{name}-%{version}

%description
Golf is a memory-safe and high-performance programming language and
application server. Golf is a compiled language that creates native 
executables. It's main purpose is easy and rapid development and 
deployment of web services and applications on Linux back-end. 
Golf is built with industry-standard Free Open Source libraries, 
extensible with C programming language.

%prep
%autosetup -n %{name}-600.3.25

%build
make clean
make GG_FEDORA_BUILD=1

%install
rm -rf %{buildroot}/*
#GG_FAKEROOT=1 means do not install selinux script to generate policy. This should be done only when sudo make install (from source)
#or when actually installing the rpm (in which case install doesn't run at all)
#GG_FAKEROOT=1 basically means this is fake root.
make DESTDIR="%{buildroot}" GG_FAKEROOT=1 GG_FEDORA_BUILD=1 install

%post
#install manual page for distros that need it (namely OpenSUSE for now)
echo "Setting up man pages..."
mandb >/dev/null 2>&1 || true
#since post runs during installation, execute selinux.setup
#sudo /usr/lib/golf/selinux/selinux.setup
#Always the last to run
echo "Setting up SELinux..."
/usr/lib/golf/selinux/selinux.setup || true

%postun
#Always the last to run
semodule -r golf || true


%files
%license LICENSE
%dir /var/lib/gg
%dir /var/lib/gg/bld/
%dir /usr/lib/golf/
%dir /usr/include/golf/
%dir /usr/lib/golf/selinux
%dir /usr/share/doc/golf
/usr/include/golf/golf.h
/usr/include/golf/gcli.h
/usr/lib/golf/libgolfarr.so
/usr/lib/golf/libgolfpg.so 
/usr/lib/golf/libgolfdb.so 
/usr/lib/golf/libgolflite.so 
/usr/lib/golf/libgolfmys.so 
/usr/lib/golf/libgolfsec.so 
/usr/lib/golf/libgolftree.so
/usr/lib/golf/libgolfcurl.so 
/usr/lib/golf/libgolfxml.so 
/usr/lib/golf/libgolfpcre2.so
/usr/lib/golf/libsrvcgolf.so 
/usr/lib/golf/libgolf.so 
/usr/lib/golf/libgolfcli.so 
/usr/lib/golf/libgolfscli.so 
/usr/lib/golf/stub_arr.o   
/usr/lib/golf/stub_after.o   
/usr/lib/golf/stub_crypto.o  
/usr/lib/golf/stub_gendb.o    
/usr/lib/golf/stub_pcre2.o     
/usr/lib/golf/stub_sqlite.o  
/usr/lib/golf/stub_tree.o
/usr/lib/golf/stub_before.o  
/usr/lib/golf/stub_curl.o    
/usr/lib/golf/stub_mariadb.o  
/usr/lib/golf/stub_postgres.o  
/usr/lib/golf/stub_srvc.o    
/usr/lib/golf/stub_xml.o
/usr/lib/golf/vmakefile
/usr/lib/golf/vdiag
/usr/lib/golf/golf.vim
/usr/lib/golf/sys
/usr/lib/golf/v1
/usr/share/doc/golf/LICENSE
/usr/share/doc/golf/NOTICE
/usr/share/doc/golf/README.md
/usr/share/doc/golf/CONTRIBUTING.md
/usr/bin/mgrg
/usr/bin/gg
/usr/share/man/man2/*.2gg*
#SELINUX
#Always distribute source selinux policy files; support for pp distribution is shaky, so none given (we generate pp files)
/usr/lib/golf/selinux/gg.te
/usr/lib/golf/selinux/golf.te
/usr/lib/golf/selinux/golf.sel
/usr/lib/golf/selinux/selinux.setup
#/usr/lib/selinux/golf.pp
#/usr/lib/selinux/gg.pp
#SELINUXEND

%changelog
* Fri Jan 31 2025 Gliim LLC <team@golf-lang.com> - 2.0.0-1
- Initial version. 
