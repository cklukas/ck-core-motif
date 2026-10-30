#!/usr/bin/env python3
"""Check FontSelector Other-mode callback state guards."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("OtherMode(Widget w")
body = source[start:source.index("/*\tFunction Name: RemoveUserError", start)]

assert "fsw == NULL || info == NULL" in body
assert body.index("if (fsw == NULL") < body.index("UnsetSiblings(w)")

print("FontSelector Other-mode callback inputs are guarded")
