#!/usr/bin/env python3
"""Check atomic pixmap-cache table initialization."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(encoding="utf-8")
start = source.index("static void \nInitializePixmapSets")
end = source.index("\n\x0c\n/************************************************************************\n *\n *  _XmCachePixmap", start)
body = source[start:end]
assert "new_pixmap_data_set" in body
assert "new_pixmap_set" in body
assert "pixmap_data_set = new_pixmap_data_set;" in body
assert "pixmap_set = new_pixmap_set;" in body
assert "_XmFreeHashTable(new_pixmap_data_set);" in body
assert body.index("pixmap_data_set = new_pixmap_data_set;") < body.index("_XmProcessUnlock")
print("Xm pixmap cache tables initialize atomically")
