#!/usr/bin/env python3
"""Check FontSelector style refresh copies slants with bounded storage."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("UpdateBoldItalic(XmFontSelectorWidget")
body = source[start:source.index("/*\tFunction Name:  UpdateFixedProportional", start)]

assert "String slant;" in body
assert "slant == NULL ? \"\" : slant" in body
assert "StoreString(slant == NULL ? \"\" : slant, cf->slant, SLANT_LEN);" in body
assert "strcpy(cf->slant" not in body

print("FontSelector slant refresh is bounded and null-safe")
