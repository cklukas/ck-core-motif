#!/usr/bin/env python3
"""Check bitmap resource failure handling in the MWM wait cursor."""

from pathlib import Path


source = Path("clients/mwm/WmFeedback.c").read_text(encoding="utf-8")
start = source.index("void ShowWaitState")
end = source.index("END OF FUNCTION ShowWaitState", start)
body = source[start:end]

assert "pixmap == None || maskPixmap == None" in body
assert "XFreePixmap (DISPLAY, pixmap)" in body
assert "XFreePixmap (DISPLAY, maskPixmap)" in body
assert body.index("pixmap == None") < body.index("XCreatePixmapCursor")

print("PASS MWM wait-cursor bitmap failures are handled")
