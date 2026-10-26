#!/usr/bin/env python3
"""Check X resource failure handling for MWM cached labels."""

from pathlib import Path


source = Path("clients/mwm/WmImage.c").read_text(encoding="utf-8")
start = source.index("Pixmap MakeCachedLabelPixmap")
end = source.index("} /* END OF FUNCTION MakeCachedLabelPixmap", start)
body = source[start:end]

assert "if (pixmap == None)" in body
assert "if (gc == NULL)" in body
assert "XFreePixmap (DISPLAY, pixmap)" in body
assert body.index("if (pixmap == None)") < body.index("XCreateGC")
assert body.index("if (gc == NULL)") < body.index("XFillRectangle")

print("PASS MWM cached-label X resource failures are handled")
