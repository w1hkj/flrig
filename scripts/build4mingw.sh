#!/bin/sh

### Script to build a win32 installation

./configure $CROSSCFG $PKGCFG FLTK_CONFIG=$PREFIX/bin/fltk-config --with-ptw32=$PREFIX/ptw32 XMLRPC_C_CONFIG=$PREFIX/bin/xmlrpc-c-config
make
i586-mingw32msvc-strip src/flrig.exe
make nsisinst
mv src/*setup*exe .


