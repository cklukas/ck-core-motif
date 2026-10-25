#!/usr/bin/env python3
"""Check _XmCachePixmap rolls back when either cache insertion fails."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("_XmCachePixmap(")
start = source.index("Boolean", start - 80)
end = source.index("\n}\n", start) + 2
function = source[start:end]

assert "cache_inserted = _XmAddHashEntry(pixmap_set" in function
assert "_XmAddHashEntry(pixmap_data_set" in function
assert "_XmRemoveHashEntry(pixmap_set, pix_entry);" in function
assert "if (!cache_inserted)" in function
failure = function.index("if (!cache_inserted)")
assert "XtFree((char *)pix_entry->image_name);" in function[failure:]
assert "XtFree((char *)pix_entry->acc_color);" in function[failure:]
assert "return (False);" in function[failure:]
print("PASS cached pixmap insertion rollback")
