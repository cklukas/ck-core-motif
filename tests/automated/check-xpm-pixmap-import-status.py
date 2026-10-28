#!/usr/bin/env python3
"""Check XPM pixmap imports propagate XGetGeometry/XGetImage failures."""
from pathlib import Path

root = Path(__file__).parents[2]
helper = (root / "lib" / "Xm" / "XpmCrIFrP.c").read_text(encoding="utf-8")
assert "int\nxpmCreateImageFromPixmap(" in helper
assert "!XGetGeometry(display, pixmap" in helper
assert "*ximage_return = XGetImage" in helper
assert "return *ximage_return == NULL ? XpmFileInvalid : XpmSuccess;" in helper

header = (root / "lib" / "Xm" / "XpmI.h").read_text(encoding="utf-8")
assert "FUNC(xpmCreateImageFromPixmap, int," in header

for name in ("XpmCrDatFrP.c", "XpmCrBufFrP.c", "XpmWrFFrP.c", "Xpmscan.c"):
    source = (root / "lib" / "Xm" / name).read_text(encoding="utf-8")
    assert "ErrorStatus = xpmCreateImageFromPixmap(" in source
    assert "goto cleanup;" in source

print("PASS XPM pixmap import failures propagate")
