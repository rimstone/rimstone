#SPDX-License-Identifier: Apache-2.0
#Copyright 2018 Gliim LLC.
#Licensed under Apache License v2. See LICENSE file.
#On the web http://golf-lang.com/ - this file is part of Golf framework.


Name:   golf
Version:    297
Release:    1%{?dist}
Summary:    Language and server for web services and back-end solutions.
Vendor:     Gliim LLC
Group:      Development/Tools
License:    Apache-2.0
Source0: https://github.com/golf-lang/%{name}/archive/%{version}/%{name}-%{version}.tar.gz 

#Make sure to install EPEL (currently just for fcgi/fcgi_devel): sudo dnf install epel-release

%define build_requires make gcc openssl-devel libcurl-devel rpm-build rpmlint pcre2-devel libxml2-devel
%define suse_build_requires %build_requires

#python utils for selinux, only for rhel
%if 0%{?rhel} 
%define rhel_sel policycoreutils-python-utils
%endif

#requirements:
#sudo dnf -y install rpmdevtools dnf-utils
#sudo dnf builddep -y fedora.spec

#OPENSUSE:sudo zypper -n install rpmdevtools yum-utils 
#OPENSUSE:sudo yum-builddep -y fedora.spec (bug with rpmutils missing, however)
#OPENSUSE has no default policy: no SELINUX
#OPENSUSE:FastCGI is for cgi-fcgi utility
%if 0%{?is_opensuse} == 1
BuildRequires: %suse_build_requires libmariadb-devel gpg sshpass FastCGI-devel postgresql-devel sqlite3-devel


%else
#MAGEIA:sudo dnf -y install rpmdevtools dnf-utils
#MAGEIA:sudo dnf builddep -y mageia.spec
#MAGEIA:fcgi is for cgi-fcgi utility
%if 0%{?mgaversion} >= 8
#SELINUX:policycoreutils policycoreutils-devel libselinux-utils
BuildRequires: %build_requires mariadb-devel libfcgi-devel postgresql-devel policycoreutils policycoreutils-devel libselinux-utils sqlite3-devel

%else
#SELINUX:policycoreutils policycoreutils-devel libselinux-utils
#fcgi is for cgi-fcgi utility; sometimes it's packaged within fcgi-devel and sometimes not.
BuildRequires: %build_requires mariadb-connector-c-devel fcgi-devel libpq-devel policycoreutils policycoreutils-devel libselinux-utils sqlite-devel
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
%autosetup -n %{name}-297

%build
make clean
make 

%install
rm -rf %{buildroot}
%if 0%{?is_opensuse} == 0
install -m 0755 -d "%{buildroot}"/usr/lib64/golf/selinux
%endif
make DESTDIR="%{buildroot}" GG_NO_SEL=1 install

#SELINUX
#This must be after make install above; otherwise /var/lib/gg wouldn't exist yet (it exists in fakeroot, but that won't work for SELINUX)
#changing context (golf.sel 1) is done again here after being done in make install, because when rpm is installed, make install works
#in a fakeroot env, and restorecon isn't really operating on /var/lib/gg; thus it is worthless for when sudo dnf install is done.
#But, when installing from source with sudo make install, i.e. without dnf, that works on real /var/lib/gg. Some redundancy but it's
#difficult to make it without.
%post
%if 0%{?is_opensuse} == 0
/usr/lib64/golf/selinux/golf.sel  "/usr/lib64/golf/selinux" "/usr/share" "/usr/bin"
exit 0
%endif
#SELINUXEND



%files
%license LICENSE
%dir /var/lib/gg
%dir /var/lib/gg/bld/
%dir /usr/lib64/golf/
%dir /usr/include/golf/
%dir /usr/share/golf/
%if 0%{?is_opensuse} == 0
%dir /usr/lib64/golf/selinux
%endif
/usr/include/golf/golf.h
/usr/include/golf/gcli.h
/usr/lib64/golf/libgolfarr.so 
/usr/lib64/golf/libgolfpg.so 
/usr/lib64/golf/libgolfdb.so 
/usr/lib64/golf/libgolflite.so 
/usr/lib64/golf/libgolfmys.so 
/usr/lib64/golf/libgolfsec.so 
/usr/lib64/golf/libgolftree.so
/usr/lib64/golf/libgolfcurl.so 
/usr/lib64/golf/libgolfxml.so 
/usr/lib64/golf/libgolfpcre2.so
/usr/lib64/golf/libsrvcgolf.so 
/usr/lib64/golf/libgolf.so 
/usr/lib64/golf/libgolfcli.so 
/usr/lib64/golf/libgolfscli.so 
/usr/lib64/golf/stub_arr.o   
/usr/lib64/golf/stub_after.o   
/usr/lib64/golf/stub_crypto.o  
/usr/lib64/golf/stub_gendb.o    
/usr/lib64/golf/stub_pcre2.o     
/usr/lib64/golf/stub_sqlite.o  
/usr/lib64/golf/stub_tree.o
/usr/lib64/golf/stub_before.o  
/usr/lib64/golf/stub_curl.o    
/usr/lib64/golf/stub_mariadb.o  
/usr/lib64/golf/stub_postgres.o  
/usr/lib64/golf/stub_srvc.o    
/usr/lib64/golf/stub_xml.o
/usr/lib64/golf/vmakefile
/usr/lib64/golf/vdiag
/usr/lib64/golf/golf.vim
/usr/lib64/golf/sys
/usr/lib64/golf/v1
/usr/bin/mgrg
/usr/bin/gg
/usr/share/man/man2/*.2gg*
/usr/share/golf/golfdoc.html
#SELINUX
#Always distribute source selinux policy files; support for pp distribution is shaky
%if 0%{?is_opensuse} == 0
/usr/lib64/golf/selinux/gg.te
/usr/lib64/golf/selinux/gg.fc
/usr/lib64/golf/selinux/golf.te
/usr/lib64/golf/selinux/golf.sel
#/usr/lib64/selinux/golf.pp
#/usr/lib64/selinux/gg.pp
%endif
#SELINUXEND

%changelog
* Fri Jan 31 2025 Gliim LLC <team@golf-lang.com> - 2.0.0-1
- Initial version. 
