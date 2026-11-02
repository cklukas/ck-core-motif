#!/bin/sh
# Keep opaque Xft/Fontconfig/FreeType objects inside one Mach-O dependency cohort.
set -eu
pkg=$1
unicode=$2
ft_dir=${3:-yes}
fc_dir=${4:-yes}
otool=${OTOOL:-otool}
fail() { echo "macOS font dependencies: $*" >&2; exit 1; }
library() {
    directory=$("$pkg" --variable=libdir "$1") || fail "missing $1 metadata"
    path=$directory/$2
    test -f "$path" || fail "missing library $path"
    printf '%s\n' "$path"
}
xft=$(library xft libXft.dylib)
case $ft_dir in yes|no|'') ft=$(library freetype2 libfreetype.dylib);; *) ft=$ft_dir/libfreetype.dylib;; esac
case $fc_dir in yes|no|'') fc=$(library fontconfig libfontconfig.dylib);; *) fc=$fc_dir/libfontconfig.dylib;; esac
test -f "$ft" && test -f "$fc" || fail "missing selected FreeType or Fontconfig library"
test "$ft" -ef "$(library freetype2 libfreetype.dylib)" || fail "selected FreeType differs from its pkg-config metadata"
test "$fc" -ef "$(library fontconfig libfontconfig.dylib)" || fail "selected Fontconfig differs from its pkg-config metadata"
dependency() {
    "$otool" -L "$1" | awk -v name="$2" 'index($1, "/" name) { print $1; exit }'
}
same_dependency() {
    actual=$(dependency "$1" "$2")
    test -n "$actual" || fail "$1 has no inspectable $2 dependency"
    test "$actual" -ef "$3" || fail "$1 uses $actual, but the selected stack uses $3. Use an Xft/Fontconfig/HarfBuzz stack built against the same FreeType library; see docs/build.md."
}
same_dependency "$xft" libfreetype. "$ft"
same_dependency "$xft" libfontconfig. "$fc"
same_dependency "$fc" libfreetype. "$ft"
x11=$(library x11 libX11.dylib)
same_dependency "$xft" libX11. "$x11"
if test "$unicode" = yes; then
    hb=$(library harfbuzz libharfbuzz.dylib)
    same_dependency "$hb" libfreetype. "$ft"
fi
printf '%s\n' "-L$(dirname "$xft") -lXft -L$(dirname "$fc") -lfontconfig -L$(dirname "$ft") -lfreetype"
