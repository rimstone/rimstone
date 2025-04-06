#SPDX-License-Identifier: Apache-2.0
#Copyright 2018 Gliim LLC.
#Licensed under Apache License v2. See LICENSE file.
#On the web http://golf-lang.com/ - this file is part of Golf framework.


Name:   golf
Version:    373
Release:    1%{?dist}
Summary:    Language and server for web services and back-end solutions.
Vendor:     Gliim LLC
Group:      Development/Tools
License:    Apache-2.0
Source0: https://github.com/golf-lang/%{name}/archive/%{version}/%{name}-%{version}.tar.gz 


#Make sure to install EPEL (currently just for fcgi/fcgi_devel): sudo dnf install epel-release

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
%define suse_requires %all_requires libmariadb-devel gpg sshpass FastCGI FastCGI-devel postgresql-devel sqlite3-devel
BuildRequires: %suse_requires 
Requires: %suse_requires 

%else
#MAGEIA:sudo dnf -y install rpmdevtools dnf-utils
#MAGEIA:sudo dnf builddep -y mageia.spec
#MAGEIA:fcgi is for cgi-fcgi utility
%if 0%{?mgaversion} >= 8
#SELINUX:policycoreutils policycoreutils-devel libselinux-utils
%define mga_requires %all_requires mariadb-devel libfcgi-devel postgresql-devel policycoreutils policycoreutils-devel libselinux-utils sqlite3-devel
BuildRequires: %mga_requires 
Requires: %mga_requires 

%else
#SELINUX:policycoreutils policycoreutils-devel libselinux-utils
#fcgi is for cgi-fcgi utility; sometimes it's packaged within fcgi-devel and sometimes not.
%define rh_requires %all_requires mariadb-connector-c-devel fcgi-devel libpq-devel policycoreutils policycoreutils-devel libselinux-utils sqlite-devel
BuildRequires: %rh_requires %{?rhel_sel}
Requires: %rh_requires %{?rhel_sel}
%endif
%endif


BuildRoot: %{_tmppath}/%{name}-%{version}
%global debug_package %{nil}

%description
Golf is a memory-safe and high-performance programming language and
application server. Golf is a compiled language that creates native 
executables. It's main purpose is easy and rapid development and 
deployment of web services and applications on Linux back-end. 
Golf is built with industry-standard Free Open Source libraries, 
extensible with C programming language.

%prep
%autosetup -n %{name}-373

%build
make clean
make 

%install
rm -rf %{buildroot}/*
#GG_FAKEROOT=1 means do not install selinux script to generate policy. This should be done only when sudo make install (from source)
#or when actually installing the rpm (in which case install doesn't run at all)
#GG_FAKEROOT=1 basically means this is fake root.
make DESTDIR="%{buildroot}" GG_FAKEROOT=1 install

%post
#since %post runs during installation, execute selinux.setup
sudo /usr/lib/golf/selinux/selinux.setup


%files
%license LICENSE
%dir /var/lib/gg
%dir /var/lib/gg/bld/
%dir /usr/lib/golf/
%dir /usr/lib/debug/golf/
%dir /usr/include/golf/
%dir /usr/lib/golf/selinux
/usr/include/golf/golf.h
/usr/include/golf/gcli.h
/usr/lib/debug/golf/libgolfarr.so.dbg 
/usr/lib/debug/golf/libgolfpg.so.dbg 
/usr/lib/debug/golf/libgolfdb.so.dbg 
/usr/lib/debug/golf/libgolflite.so.dbg 
/usr/lib/debug/golf/libgolfmys.so.dbg 
/usr/lib/debug/golf/libgolfsec.so.dbg 
/usr/lib/debug/golf/libgolftree.so.dbg
/usr/lib/debug/golf/libgolfcurl.so.dbg 
/usr/lib/debug/golf/libgolfxml.so.dbg 
/usr/lib/debug/golf/libgolfpcre2.so.dbg
/usr/lib/debug/golf/libsrvcgolf.so.dbg 
/usr/lib/debug/golf/libgolf.so.dbg 
/usr/lib/debug/golf/libgolfcli.so.dbg 
/usr/lib/debug/golf/libgolfscli.so.dbg 
/usr/lib/debug/golf/v1.dbg
/usr/lib/debug/golf/mgrg.dbg
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
/usr/lib/golf/LICENSE
/usr/lib/golf/NOTICE
/usr/lib/golf/README.md
/usr/lib/golf/CONTRIBUTING.md
/usr/bin/mgrg
/usr/bin/gg
/usr/share/man/man2/*.2gg*
#SELINUX
#Always distribute source selinux policy files; support for pp distribution is shaky, so none given (we generate pp files)
/usr/lib/golf/selinux/gg.te
/usr/lib/golf/selinux/gg.fc
/usr/lib/golf/selinux/golf.te
/usr/lib/golf/selinux/golf.sel
/usr/lib/golf/selinux/selinux.setup
#/usr/lib/selinux/golf.pp
#/usr/lib/selinux/gg.pp
#SELINUXEND

%changelog
* Fri Jan 31 2025 Gliim LLC <team@golf-lang.com> - 2.0.0-1
- Initial version. 
