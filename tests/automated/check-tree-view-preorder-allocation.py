#!/usr/bin/env python3
"""Check TreeView preorder scratch storage is size-bounded."""

from pathlib import Path


source = Path("lib/Xm/TreeView.c").read_text()
start = source.index("static Cardinal *input_preorder(")
body = source[start:source.index("static int depth_of", start)]

assert "#if SIZE_MAX < UINT_MAX" in body
assert "(size_t)count > SIZE_MAX / sizeof(*order)" in body
assert "count > UINT_MAX / sizeof(*order)" in body
assert "bytes = (size_t)count * sizeof(*order);" in body
assert "XtMalloc((Cardinal)bytes)" in body
assert body.count("XtMalloc((Cardinal)bytes)") == 2

print("TreeView preorder scratch storage is allocation-bounded")
