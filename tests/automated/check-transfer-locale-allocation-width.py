#!/usr/bin/env python3
"""Ensure locale-text allocation uses its validated Cardinal width."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("_XmTextToLocaleText(Widget")
end = source.index("\nvoid\n_XmConvertComplete", start)
body = source[start:end]

assert "malloc_size >= (size_t)UINT_MAX" in body
assert "XtMalloc ((Cardinal)(malloc_size + 1))" in body
assert "XtMalloc (malloc_size + 1)" not in body

print("PASS locale-text allocation width is explicit")
