#!/bin/sh
# Run after configure and config/util/makestrs have been built.
# Exercise all missing outputs, one missing output, and a true no-op.
set -eu
build=$(cd "$1" && pwd)
cd "$build/lib/Xm"
outputs="XmStrDefs.h XmStrDefs22.h XmStrDefs23.h XmStrDefsI.h XmStrDefs.c"
rm -f $outputs
make -j4 $outputs
for f in $outputs; do test -s "$f"; done
sha256sum $outputs > "$build/strings-before.sha256"
rm XmStrDefs22.h
make -j4 $outputs
sha256sum -c "$build/strings-before.sha256"
make -q $outputs
printf '%s\n' 'PASS: grouped string generation, missing-output recovery, no-op'
