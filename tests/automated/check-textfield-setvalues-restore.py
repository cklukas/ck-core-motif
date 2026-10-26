#!/usr/bin/env python3
"""Check that SetValues restoration never copies through a NULL realloc."""

from pathlib import Path


source = Path("lib/Xm/TextF.c").read_text(encoding="utf-8")

assert "memcpy(XtRealloc(TextF_Value(new_tf)" not in source
assert "memcpy( XtRealloc((char *)TextF_WcValue(new_tf)" not in source
assert "char *restored = (char *)XtRealloc(TextF_Value(new_tf)" in source
assert "wchar_t *restored = (wchar_t *)XtRealloc(" in source
assert "if (restored != NULL)" in source

print("TextField SetValues restoration checks passed")
