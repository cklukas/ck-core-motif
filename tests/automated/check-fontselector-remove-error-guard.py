#!/usr/bin/env python3
"""Check FontSelector text-error callback state guards."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("RemoveUserError(Widget w")
body = source[start:source.index("/************************************************************", start)]

assert "if (fsw == NULL)" in body
assert body.index("if (fsw == NULL)") < body.index(
    "XmFontS_current_text(fsw)")

print("FontSelector text-error callback handles null state")
