# build file to generate the distribution binary tarball
myscripts/cleanup

autoreconf

./configure --prefix=/tmp/flrig-build --enable-static
make install-strip
tar czf flrig-$1.bin.tgz -C /tmp/flrig-build .

make clean

./configure \
  $PKGCFG \
  $CROSSCFG \
  --without-asciidoc \
  --with-ptw32=/opt/mxe/usr/i686-pc-mingw32 \
  --with-libiconv-prefix=$PREFIX/iconv \
  --enable-static \
  --with-libintl-prefix=$PREFIX/gettext \
  PTW32_LIBS="-lpthread -lpcreposix -lpcre -lregex" \
  FLTK_CONFIG=$PREFIX/bin/i686-pc-mingw32-fltk-config \

make
$PREFIX/bin/i686-pc-mingw32-strip src/flrig.exe
make nsisinst
mv src/*setup*exe .

make clean

# build the distribution tarball
./configure
make distcheck
make clean
