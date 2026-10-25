#!/usr/bin/env python3
"""Check Xm image-install allocation and initialization guards."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(encoding="utf-8")
start = source.index("_XmInstallImage(")
end = source.index("/************************************************************************\n *\n *  XmInstallImage", start)
body = source[start:end]
assert "if (image_set == NULL)" in body
assert "if (entry == NULL)" in body
assert "if (entry->image_name == NULL)" in body
assert "XtFree((char *)entry);" in body
assert body.count("_XmProcessUnlock();") >= 4
print("Xm image installation handles initialization and allocation failures")
