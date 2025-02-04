# SPDX-License-Identifier: Apache-2.0
# Copyright 2018 Gliim LLC. 
# Licensed under Apache License v2. See LICENSE file.
# On the web http://golf-lang.com/ - this file is part of Golf framework.

# Maintainer: Gliim LLC <golf-lang@proton.me>
pkgname=golf
pkgver=216
pkgrel=0
epoch=
pkgdesc="Golf is language, tools and application server for web services and web application, high performance and memory-safe"
arch=("x86_64")
url="https://golf-lang.com"
license=('Apache-2.0')
groups=()
depends=(make gcc openssl curl 'mariadb-connector-c' fcgi 'postgresql-libs' sqlite3 pcre2 libxml2)
makedepends=(make gcc openssl curl 'mariadb-connector-c' fcgi 'postgresql-libs' sqlite3 pcre2 libxml2)
checkdepends=()
optdepends=()
provides=(golf)
conflicts=()
replaces=()
backup=()
options=()
install=
changelog=
source=("https://github.com/golf-lang/golf/archive/refs/tags/216.tar.gz")
        
noextract=()
md5sums=(SKIP)
validpgpkeys=()

prepare() {
	tar xvfz 216.tar.gz 
}

build() {
	cd "golf-216"
	make DESTDIR="$pkgdir/" clean
	make DESTDIR="$pkgdir/"
}

check() {
	cd "golf-216"
}

package() {
	cd "golf-216"
	make DESTDIR="$pkgdir/" install
}
