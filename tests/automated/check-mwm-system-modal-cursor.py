#!/usr/bin/env python3
"""Check system-modal cursor resource handling in MWM."""

from pathlib import Path


source = Path("clients/mwm/WmWinList.c").read_text(encoding="utf-8")
start = source.index("void SetupSystemModalState")
end = source.index("END OF FUNCTION SetupSystemModalState", start)
body = source[start:end]

assert "windowAttributes.cursor = None;" in body
assert "pixmap != None && maskPixmap != None" in body
assert body.index("pixmap != None && maskPixmap != None") < body.index("XCreatePixmapCursor")
assert "if (pixmap != None)" in body
assert "if (maskPixmap != None)" in body

print("PASS MWM system-modal cursor resources are guarded")
