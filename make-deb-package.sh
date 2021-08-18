#!/bin/sh

make
mkdir -p pkg-debian/DEBIAN
mkdir -p pkg-debian/usr/include
mkdir -p pkg-debian/usr/lib
echo """Package: libsevaluator
Version: 1.0.0
Architecture: amd64
Essential: no
Section: utils
Priority: optional
Depends: libgmp10, libmpfr6, libgmp10-dev, libmpfr-dev
Maintainer: nth233<fpg2012@yeah.net>
Installed-Size: 1
Description: simple/super expression evaluation library with big number support.""" > pkg-debian/DEBIAN/control
cp *.so -t pkg-debian/usr/lib/
cp *.h -t pkg-debian/usr/include/
dpkg -b pkg-debian/ libsevaluator_amd64.deb
rm -r pkg-debian
make clean
