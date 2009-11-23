#!/bin/sh

### Script to build a win32 installation

./configure $CROSSCFG $PKGCFG FLTK_CONFIG=$PREFIX/bin/fltk-config
make
i586-mingw32msvc-strip src/flwrap.exe
make nsisinst


