#!/usr/bin/env python3
"""Check allocation-failure cleanup in _XmCachePixmap."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(encoding="utf-8")
start = source.index("_XmCachePixmap(")
end = source.index("\n\n\n/*******************************************************************/", start)
body = source[start:end]
assert "if (pixmap_data_set == NULL || pixmap_set == NULL)" in body
assert "if (pix_entry == NULL)" in body
assert "if (pix_entry->acc_color == NULL)" in body
assert "if (pix_entry->image_name == NULL)" in body
assert body.count("return (False);") >= 6
assert body.index("if (pix_entry == NULL)") < body.index("pix_entry->screen")
assert body.index("if (pix_entry->acc_color == NULL)") < body.index("pix_entry->acc_color->foreground")
assert body.index("if (pix_entry->image_name == NULL)") < body.index("pix_entry->pixmap")
print("Xm pixmap-cache allocations fail cleanly")
