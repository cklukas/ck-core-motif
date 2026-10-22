#!/bin/sh
# Build with --enable-xft first. Check that configuration and two negative cases.
set -eu
src=$(cd "$1" && pwd)
root=$2
enabled=$(cd "$3" && pwd)
mkdir "$root"
root=$(cd "$root" && pwd)
mkdir "$root/tmp" "$root/disabled" "$root/missing"
export TMPDIR="$root/tmp"
for feature in USE_XFT HAVE_FREETYPE HAVE_FONTCONFIG HAVE_XRENDER; do
    grep -q "^#define $feature 1$" "$enabled/include/config.h"
done
(cd "$root/disabled" && PKG_CONFIG=/bin/false "$src/configure" --disable-xft --disable-static --disable-printing >configure.log 2>&1)
if grep -q '^#define USE_XFT 1$' "$root/disabled/include/config.h"; then exit 1; fi
if (cd "$root/missing" && PKG_CONFIG=/bin/false "$src/configure" --enable-xft --disable-static --disable-printing >configure.log 2>&1); then
    echo 'FAIL: requested Xft silently accepted missing dependencies' >&2
    exit 1
fi
grep -q 'Xft requires pkg-config metadata' "$root/missing/configure.log"
printf '%s\n' 'PASS: Xft enabled probes, disabled support and missing-dependency rejection'
