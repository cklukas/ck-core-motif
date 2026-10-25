#!/usr/bin/env python3
"""Check Xm null-cursor resource failure handling."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Screen.c").read_text(encoding="utf-8")
start = source.index("XmeGetNullCursor(")
end = source.index("/*\n * The following set", start)
body = source[start:end]
assert "if (pixmap == XmUNSPECIFIED_PIXMAP)" in body
assert "if (cursor == None)" in body
assert body.count("_XmAppUnlock(app);") >= 2
assert "xmScreen->screen.nullCursor = cursor;" in body
print("Xm null-cursor creation failures leave no invalid cached cursor")
