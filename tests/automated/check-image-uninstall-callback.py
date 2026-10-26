#!/usr/bin/env python3
"""Check that image uninstall hash callbacks validate entry and image."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.rindex("UninstallImageMapProc (XmHashKey")
end = source.index("\n}\n", start) + 3
region = source[start:end]
assert "entry == NULL || image == NULL" in region
print("image uninstall callback validates entry and image inputs")
