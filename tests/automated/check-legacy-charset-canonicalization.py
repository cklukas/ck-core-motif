#!/usr/bin/env python3
"""Ensure legacy charset canonicalization bounds copied names."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Obso1_2.c"
text = source.read_text(encoding="utf-8")
start = text.index("_XmCharsetCanonicalize(")
end = text.index("\n/*ARGSUSED*/", start)
block = text[start:end]

assert "size_t\t\tlen;" in block
assert "len > (size_t)UINT_MAX - 1" in block
assert "XtMalloc((Cardinal)(len + 1))" in block

print("legacy charset canonicalization bounds: ok")
