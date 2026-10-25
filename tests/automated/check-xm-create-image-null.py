#!/usr/bin/env python3
"""Check null-safe XImage construction in Motif image paths."""

from pathlib import Path

root = Path(__file__).parents[2]
image_cache = (root / "lib" / "Xm" / "ImageCache.c").read_text(encoding="utf-8")
for header in ("XmI.h", "XmP.h"):
    macro = (root / "lib" / "Xm" / header).read_text(encoding="utf-8")
    assert "if (IMAGE != NULL)" in macro
    assert "IMAGE->byte_order" in macro
start = image_cache.index("if (!built_in_image)")
end = image_cache.index("built_in_image->data", start)
body = image_cache[start:end]
assert "_XmCreateImage(built_in_image" in body
assert "if (!built_in_image)" in body
assert "return FALSE;" in body
print("Xm XImage construction handles null allocation results")
