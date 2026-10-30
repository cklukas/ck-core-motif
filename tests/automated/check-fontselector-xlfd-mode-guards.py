#!/usr/bin/env python3
"""Check FontSelector XLFD-mode callback state guards."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("XlfdMode(Widget w")
body = source[start:source.index("/*\tFunction Name: OtherMode", start)]

assert "fsw == NULL || info == NULL" in body
assert body.index("if (fsw == NULL") < body.index("UnsetSiblings(w)")

print("FontSelector XLFD-mode callback inputs are guarded")
