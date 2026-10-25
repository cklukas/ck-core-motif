#!/usr/bin/env python3
"""Check resolved image-path cleanup when fopen fails."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(encoding="utf-8")
start = source.index("if (!(infile = fopen(file_name, \"rb\")))")
end = source.index("#ifdef JPEG_SUPPORTED", start)
body = source[start:end]
assert "XtFree(file_name);" in body
assert "return FALSE;" in body
assert body.index("XtFree(file_name);") < body.index("return FALSE;")
print("Xm image loading frees resolved paths when fopen fails")
