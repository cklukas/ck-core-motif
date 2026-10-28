#!/usr/bin/env python3
"""Check accessibility Unicode scans reject int32-inexpressible byte ranges."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "AccessibleDBus.c").read_text()
start = source.index("byte_length = strlen(source);")
end = source.index("map = XmTextUnicodeMapCreate", start)
body = source[start:end]

assert "byte_length > (size_t)INT32_MAX" in body
assert "XtFree(source);" in body
assert "return 0;" in body

print("PASS accessibility UTF-8 byte-range guard")
