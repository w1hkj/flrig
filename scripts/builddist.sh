# build file to generate the distribution binary tarball
autoreconf

# build windows installation file
./configure \
  $PKGCFG \
  --host=i686-w64-mingw32.static \
  --with-ptw32=$PREFIX/i686-w64-mingw32.static \
  --with-libiconv-prefix=$PREFIX/iconv \
  --enable-static \
  PTW32_LIBS="-lpthread -lpcreposix -lpcre -lregex" \
  FLTK_CONFIG=$PREFIX/i686-w64-mingw32.static/bin/fltk-config

make -j 3
$PREFIX/bin/i686-w64-mingw32.static-strip src/flrig.exe
make nsisinst
mv src/*setup*exe .

make clean

# build the distribution tarball
./configure
make distcheck
make clean
