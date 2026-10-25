#!/usr/bin/env python3
"""Check serialized Xm pixmap-cache table initialization."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(encoding="utf-8")
start = source.index("static void \nInitializePixmapSets( void )")
end = source.index("/************************************************************************\n *\n *  _XmCachePixmap", start)
body = source[start:end]
assert "_XmProcessLock();" in body
assert "if (pixmap_data_set != NULL || pixmap_set != NULL)" in body
assert body.index("if (pixmap_data_set != NULL || pixmap_set != NULL)") < body.index("_XmAllocHashTable")
assert "_XmProcessUnlock();" in body
assert "pixmap_data_set = new_pixmap_data_set;" in body
assert "pixmap_set = new_pixmap_set;" in body
print("Xm pixmap cache tables initialize once under the process lock")
