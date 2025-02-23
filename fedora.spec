#SPDX-License-Identifier: Apache-2.0
#Copyright 2018 Gliim LLC.
#Licensed under Apache License v2. See LICENSE file.
#On the web http://golf-lang.com/ - this file is part of Golf framework.

Name:   golf
Version:    258
Release:    1%{?dist}
Summary:    Programming language, tools and server for web services and applications
Vendor:     Gliim LLC

Group:      Development/Tools
License:    Apache-2.0
Source0: https://github.com/golf-lang/%{name}/archive/%{version}/%{name}-%{version}.tar.gz 

#Make sure to install EPEL (currently just for fcgi/fcgi_devel): sudo dnf install epel-release

%define build_requires make gcc openssl-devel libcurl-devel rpm-build createrepo rpmlint pcre2-devel
#due to a bug in building dependencies for opensuse, must have this to build a dependency statement via bash script
#suse_build_requires *must* be identical to build_requires - do *not* change its definition
%define suse_build_requires %build_requires
%define run_requires make gcc openssl-devel libcurl-devel pcre2-devel

#devel lib for mariadb
%if 0%{?el9} 
%define maria_devel mariadb-connector-c-devel
%else
%define maria_devel mariadb-devel
%endif

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
%if 0%{?suse_version} >= 1500
BuildRequires: %suse_build_requires libmariadb-devel gpg sshpass FastCGI-devel postgresql-devel sqlite3-devel
Requires:    %run_requires libmariadb-devel FastCGI FastCGI-devel postgresql-devel sqlite3-devel


%else
#MAGEIA:sudo dnf -y install rpmdevtools dnf-utils
#MAGEIA:sudo dnf builddep -y mageia.spec
#MAGEIA:fcgi is for cgi-fcgi utility
%if 0%{?mgaversion} >= 8
#SELINUX:policycoreutils policycoreutils-devel libselinux-utils
BuildRequires: %build_requires mariadb-devel libfcgi-devel postgresql-devel policycoreutils policycoreutils-devel libselinux-utils sqlite3-devel
Requires:    %run_requires mariadb-devel fcgi libfcgi-devel apache-mod_proxy libpq-devel policycoreutils policycoreutils-devel libselinux-utils sqlite3-devel


%else
#SELINUX:policycoreutils policycoreutils-devel libselinux-utils
#fcgi is for cgi-fcgi utility; sometimes it's packaged within fcgi-devel and sometimes not.
BuildRequires: %build_requires %{?maria_devel} fcgi-devel libpq-devel policycoreutils policycoreutils-devel libselinux-utils sqlite-devel
Requires:    %run_requires %{?maria_devel} %{?rhel_sel} fcgi fcgi-devel libpq-devel policycoreutils policycoreutils-devel libselinux-utils sqlite-devel
%endif
%endif


BuildRoot: %{_tmppath}/%{name}-%{version}
%global debug_package %{nil}

%description 
Golf is a programming language and  application server for building web services and back-end solutions on Linux. Golf is easy to develop with, memory-safe and high-performance. 

%prep
%autosetup -n %{name}-258

#For faster building on Fedora 38, you can define special_build to yes. In this case, a step prior to rpmbuild must
#actually make Golf in $HOME/golf directory and build it in $HOME/golf/build directory. Otherwise, do not use this flag.
%build
%if "%{special_build}" == "yes"
cp ~/golf/* . || true
%else
make clean
make 
%endif

%install
%if "%{special_build}" == "yes"
rm -rf %{buildroot}
mkdir -p %{buildroot}
cp -rf ~/golf/build/* "%{buildroot}" || true
%else
rm -rf %{buildroot}
make DESTDIR="%{buildroot}" GG_NO_SEL=1 install
%endif

#SELINUX
#This must be after make install above; otherwise /var/lib/gg wouldn't exist yet (it exists in fakeroot, but that won't work for SELINUX)
#changing context (golf.sel 1) is done again here after being done in make install, because when rpm is installed, make install works
#in a fakeroot env, and restorecon isn't really operating on /var/lib/gg; thus it is worthless for when sudo dnf install is done.
#But, when installing from source with sudo make install, i.e. without dnf, that works on real /var/lib/gg. Some redundancy but it's
#difficult to make it without.
%post
%if 0%{?suse_version}  == 0
"%{_libdir}"/golf/selinux/golf.sel  "%{_libdir}/golf/selinux" "%{_datadir}" "%{_bindir}"
exit 0
%endif
#SELINUXEND



%files
%license LICENSE
%dir /var/lib/gg/bld/
%dir %{_libdir}/golf/
%dir %{_includedir}/golf/
%dir %{_datadir}/golf/
%{_includedir}/golf/golf.h
%{_includedir}/golf/gcli.h
%{_libdir}/golf/libgolfarr.so 
%{_libdir}/golf/libgolfpg.so 
%{_libdir}/golf/libgolfdb.so 
%{_libdir}/golf/libgolflite.so 
%{_libdir}/golf/libgolfmys.so 
%{_libdir}/golf/libgolfsec.so 
%{_libdir}/golf/libgolftree.so
%{_libdir}/golf/libgolfcurl.so 
%{_libdir}/golf/libgolfxml.so 
%{_libdir}/golf/libgolfpcre2.so
%{_libdir}/golf/libgolfpcre2glibc.so
%{_libdir}/golf/libsrvcgolf.so 
%{_libdir}/golf/libgolf.so 
%{_libdir}/golf/libgolfcli.so 
%{_libdir}/golf/libgolfscli.so 
%{_libdir}/golf/stub_arr.o   
%{_libdir}/golf/stub_after.o   
%{_libdir}/golf/stub_crypto.o  
%{_libdir}/golf/stub_gendb.o    
%{_libdir}/golf/stub_pcre2.o     
%{_libdir}/golf/stub_sqlite.o  
%{_libdir}/golf/stub_tree.o
%{_libdir}/golf/stub_before.o  
%{_libdir}/golf/stub_curl.o    
%{_libdir}/golf/stub_mariadb.o  
%{_libdir}/golf/stub_postgres.o  
%{_libdir}/golf/stub_srvc.o    
%{_libdir}/golf/stub_xml.o
%{_libdir}/golf/vmakefile
%{_libdir}/golf/vdiag
%{_libdir}/golf/golf.vim
%{_libdir}/golf/sys
%{_libdir}/golf/pcre2_version
%{_libdir}/golf/pcre2_libs
%{_libdir}/golf/v1
%{_bindir}/mgrg
%{_bindir}/gg
%{_mandir}/man2/*.2gg*
%{_datadir}/golf/golfdoc.html
#SELINUX
#Always distribute source selinux policy files; support for pp distribution is shaky
%if 0%{?suse_version}  == 0
%dir %{_libdir}/golf/selinux
%{_libdir}/golf/selinux/gg.te
%{_libdir}/golf/selinux/gg.fc
%{_libdir}/golf/selinux/golf.te
%{_libdir}/golf/selinux/golf.sel
#%{_libdir}/selinux/golf.pp
#%{_libdir}/selinux/gg.pp
%endif
#SELINUXEND

%changelog
* Fri Jan 31 2025 Gliim LLC <golf-lang@proton.me> - 2.0.0-1
- Initial version. 
