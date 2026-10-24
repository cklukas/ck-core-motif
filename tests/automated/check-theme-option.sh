#!/bin/sh
set -eu

grep -F 'AC_MSG_ERROR([--enable-themes was requested' configure.ac >/dev/null
if grep -F 'enable_themes="no"' configure.ac >/dev/null; then
    echo "theme option still silently disables itself" >&2
    exit 1
fi
echo "PASS --enable-themes fails clearly when implementation is absent"
