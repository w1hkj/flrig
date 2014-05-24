#!/bin/sh

# build windows installation file

./configure \
  $PKGCFG \
  $CROSSCFG \
  --with-ptw32=/opt/mxe/usr/i686-pc-mingw32 \
  --with-libiconv-prefix=$PREFIX/iconv \
  --enable-static \
  PTW32_LIBS="-lpthread -lpcreposix -lpcre -lregex" \
  FLTK_CONFIG=$PREFIX/bin/i686-pc-mingw32-fltk-config

make
$PREFIX/bin/i686-pc-mingw32-strip src/flrig.exe
make nsisinst
mv src/*setup*exe .

