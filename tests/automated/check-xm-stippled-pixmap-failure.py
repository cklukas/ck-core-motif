#!/usr/bin/env python3
"""Check Xm stippled-pixmap allocation and release guards."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmExtUtil.c").read_text(encoding="utf-8")
start = source.index("XiCreateStippledPixmap")
end = source.index("void \nXiReleaseStippledPixmap", start)
create = source[start:end]
release = source[end:]
assert "if (screen == NULL)" in create
assert "if (stippled_pixmap == XmUNSPECIFIED_PIXMAP)" in create
assert "if (cachePtr == NULL)" in create
assert "XFreePixmap(display, stippled_pixmap);" in create
assert "if (screen == NULL || pixmap == XmUNSPECIFIED_PIXMAP)" in release
print("Xm stippled-pixmap failures are handled without caching invalid resources")
