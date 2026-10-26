#!/usr/bin/env python3
"""Check X resource failure handling in MWM icon creation."""

from pathlib import Path


source = Path("clients/mwm/WmImage.c").read_text(encoding="utf-8")
start = source.index("Pixmap MakeIconPixmap")
end = source.index("} /* END OF FUNCTION MakeIconPixmap", start)
body = source[start:end]

assert "if (iconPixmap == None)" in body
assert "if (imageGC == NULL)" in body
assert "XFreePixmap (DISPLAY, iconPixmap)" in body
assert body.index("if (iconPixmap == None)") < body.index("XCreateGC")
assert body.index("if (imageGC == NULL)") < body.index("XFillRectangle")

print("PASS MWM icon X resource failures are handled")
