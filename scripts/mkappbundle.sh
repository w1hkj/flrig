#!/bin/sh

### Script to create the .app structure for osx
### 20180201  David Freese W1HKJ

if [ $# -ne 2 ]; then
	echo "Syntax: $0 data-dir build-dir" >&2
	exit 1
fi

upcase1()
{
	sed 'h; s/\(^.\).*/\1/; y/abcdefghijklmnopqrstuvwxyz/ABCDEFGHIJKLMNOPQRSTUVWXYZ/; G; s/\n.//'
}

function copy_libs()
{
	list="$1"
	while test "x$list" != "x"; do
	change="$list"
	list=""

	for obj in $change; do
		for lib in `otool -L $obj | \
			sed -n 's!^.*[[:space:]]\([^[:space:]]*\.dylib\).*$!\1!p' | \
				grep -Ev '^/(usr/lib|System)'`; do
		libfn="`basename $lib`"
		if ! test -f "Frameworks/$libfn"; then
			cp "$lib" "Frameworks/$libfn"
			install_name_tool -id "@executable_path/../Frameworks/$libfn" "Frameworks/$libfn"
			list="$list Frameworks/$libfn"
		fi
		install_name_tool -change "$lib" "@executable_path/../Frameworks/$libfn" "$obj"
		done
	done
	done
}

function bundle()
{
	appname="${binary}-${appversion}.app"

	cd "$build"

	echo "creating ${build}/$bundle_dir/$appname"

	$mkinstalldirs "$bundle_dir/$appname/Contents/MacOS" "$bundle_dir/$appname/Contents/Resources"
	cd "$bundle_dir"

	$INSTALL_PROGRAM "${build}/$binary" "$appname/Contents/MacOS"
	test "x$NOSTRIP" = "x" && ${STRIP:-strip} -S "$appname/Contents/MacOS/$binary"
	$INSTALL_DATA "$icon" "$appname/Contents/Resources"

	echo "APPL${signature}" > "$appname/Contents/PkgInfo"
	sed -e "s!%%IDENTIFIER%%!${identifier}!g; s!%%NAME%%!${name}!g;\
		s!%%SIGNATURE%%!${signature}!g; s!%%BINARY%%!${binary}!g;\
		s!%%VERSION%%!${version}!g; s!%%ICON%%!${icon##*/}!g;" < "$plist" > "$appname/Contents/Info.plist"

	if grep '%%[A-Z]*%%' "$appname/Contents/Info.plist"; then
		echo "E: unsubstituted variables in $appname/Contents/Info.plist" >&2
	exit 1
	fi

	$mkinstalldirs "$appname/Contents/Frameworks"
	cd "$appname/Contents"

	copy_libs "MacOS/$binary"

}

#=======================================================================

if [ -z "$PACKAGE_TARNAME" ]; then
	echo "E: \$PACKAGE_TARNAME undefined"
	exit 1
fi

PWD=`pwd`
data="${PWD}/$1"
build="${PWD}/$2"
bundle_dir="$APPBUNDLE"

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

plist="${data}/mac/Info.plist.in"
flrig_icon="${data}/mac/flrig.icns"
for f in "$plist" "$flrig_icon"; do
	test -r "$f" && continue
	echo "E: ${f}: not readable" >&2
	exit 1
done

set -e

identifier="com.w1hkj.$PACKAGE_TARNAME"
name=$(echo "$PACKAGE_TARNAME" | upcase1)

# we'll use the first four consonants as the signature

signature="$(echo $PACKAGE_TARNAME | sed 's/[aeiouAEIOU]//g; s/\(^....\).*/\1/')"

signature="$PACKAGE_TARNAME"
binary="$PACKAGE_TARNAME"
icon="$flrig_icon"
version="${FLRIG_VERSION_MAJOR}.${FLRIG_VERSION_MINOR}.${FLRIG_VERSION_PATCH}"
appversion="$PACKAGE_VERSION"

bundle

cd "$build"

echo "creating disk image"

echo "    source: " $bundle_dir
echo "    target: " ${APPBUNDLE}.dmg

hdiutil create -ov -srcfolder "$bundle_dir" -format UDZO -tgtimagekey zlib-level=9 "${APPBUNDLE}.dmg"
