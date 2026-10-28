#!/usr/bin/env python3
"""Check TextField modify-verify callbacks use checked size_t conversions."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("static Boolean \nModifyVerify(")
end = source.index("static void \nResetClipOrigin", start)
body = source[start:end]

assert "size_t count;" in body
assert body.count("count == (size_t)-1") >= 4
assert "count > (size_t)INT_MAX" in body
assert "count < 0" not in body
assert "wcs_newblock.length = mbstowcs" not in body
assert "count = (int) wcstombs" not in body

print("PASS TextField modify-verify conversion guards")
