#!/usr/bin/env python3
"""Check FontS family refresh validates its temporary XmString array."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FontS.c").read_text()
start = source.index("UpdateFamilies(XmFontSelectorWidget fsw)")
body = source[start:source.index("/*\tFunction Name:", start + 20)]
assert "SIZE_MAX / sizeof(*strs)" in body
assert "strs == NULL && num != 0" in body
