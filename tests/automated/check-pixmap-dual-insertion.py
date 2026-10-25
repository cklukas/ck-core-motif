#!/usr/bin/env python3
"""Check scaled pixmap cache publication is transactional across both tables."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("/* put the new entry in both tables")
end = source.index("\n    /*  Set up a gc", start)
region = source[start:end]

assert "cache_inserted = _XmAddHashEntry(pixmap_set" in region
assert "if (cache_inserted &&" in region
assert "_XmAddHashEntry(pixmap_data_set" in region
assert "_XmRemoveHashEntry(pixmap_set, pix_entry);" in region
assert "if (!cache_inserted)" in region
assert "XFreePixmap(display, pixmap);" in region
assert region.index("_XmProcessUnlock();") < region.index("if (!cache_inserted)")
print("PASS scaled pixmap dual-table insertion rollback")
