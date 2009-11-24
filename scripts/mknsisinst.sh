#!/bin/sh

### Script to create a win32 installer file
### 20090510  Stelios Bounanos M0GLD, Dave Freese W1HKJ

### change for the target application
FLRIG_name=flrig
FLRIG_bin=flrig.exe

if [ $# -ne 2 ]; then
    echo "Syntax: $0 data-dir build-dir" >&2
    exit 1
fi

if [ -z "$PACKAGE_TARNAME" ]; then
    echo "E: \$PACKAGE_TARNAME undefined"
    exit 1
fi

PWD=`pwd`
data="${PWD}/$1"
build="${PWD}/$2"

# more sanity checks
for d in "$data" "$build"; do
    test -d "$d" && continue
    echo "E: ${d}: not a directory" >&2
    exit 1
done
if ! test -w "$build"; then
    echo "E: ${build} is not writeable" >&2
    exit 1
fi

set -e

test "x$NOSTRIP" = "x" && $STRIP -S "$FLRIG_bin"
def="$def -DFLRIG_NAME=$FLRIG_name -DFLRIG_BINARY=$FLRIG_bin -DFLRIG_VERSION=$PACKAGE_VERSION"

# Look for pthreadGC2.dll and mingwm10.dll
MINGWM_DLL=mingwm10.dll
PTW32_DLL=pthreadGC2.dll
if ! test -r "$build/$MINGWM_DLL" || ! test -r "$build/$PTW32_DLL"; then
    IFS_saved="$IFS"
    IFS=:
    MINGWM_PATH=""
    PTW32_PATH=""
    for dir in $LIB_PATH; do
	test "x$MINGWM_PATH" = "x" && test -r "$dir/$MINGWM_DLL" && MINGWM_PATH="$dir/$MINGWM_DLL"
	test "x$PTW32_PATH" = "x" && test -r "$dir/$PTW32_DLL" && PTW32_PATH="$dir/$PTW32_DLL"
    done
    IFS="$IFS_saved"
fi
if ! test -r "$build/$MINGWM_DLL"; then
    if test "x$MINGWM_PATH" != "x"; then
	cp "$MINGWM_PATH" "$build"
    elif test -r /usr/share/doc/mingw32-runtime/${MINGWM_DLL}.gz; then
        # Debian and Ubuntu
	gzip -dc /usr/share/doc/mingw32-runtime/${MINGWM_DLL}.gz > "$build/$MINGWM_DLL"
    fi
fi
if ! test -r "$build/$PTW32_DLL"; then
    if test "x$PTW32_PATH" != "x"; then
	cp "$PTW32_PATH" "$build"
    else
    # look for dll in PTW32_LIBS
	dir=$(echo $PTW32_LIBS | sed -r 's/.*-L([[:graph:]]+).*/\1/g')
	lib=$(echo $PTW32_LIBS | sed -r 's/.*-l(pthreadGC[[:graph:]]+).*/\1/g')
	lib="${lib}.dll"
	if test -r "$dir/$lib"; then
	    cp "$dir/$lib" "$build"
	fi
    fi
fi
def="$def -DMINGWM_DLL=$MINGWM_DLL -DPTW32_DLL=$PTW32_DLL"

$MAKENSIS -V2 -NOCD -D"INSTALLER_FILE=$INSTALLER_FILE" -D"LICENSE_FILE=$data/../COPYING" \
    -D"SUPPORT_URL=$PACKAGE_HOME" -D"UPDATES_URL=$PACKAGE_DL" $def "$data/win32/fl_app.nsi"
