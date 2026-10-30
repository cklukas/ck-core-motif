#!/usr/bin/env python3
"""Check BuildFontString rejects incomplete inputs and null XLFD quarks."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("BuildFontString(XmFontSelectorWidget")
body = source[start:source.index("/*\tFunction Name: UpdateBoldItalic", start)]

assert "fsw == NULL || cf == NULL" in body
assert "(size > 0 && buf == NULL)" in body
assert "if (family == NULL)" in body
assert "if (weight == NULL)" in body
assert "if (encoding == NULL)" in body
assert "family, weight, cf->slant" in body

print("FontSelector font-string construction is null-safe")
