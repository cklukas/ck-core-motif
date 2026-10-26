#!/usr/bin/env python3
"""Check bounds and input validation for MWM cached-label lookup."""

from pathlib import Path


source = Path("clients/mwm/WmImage.c").read_text(encoding="utf-8")
start = source.index("Pixmap MakeCachedLabelPixmap")
end = source.index("} /* END OF FUNCTION MakeCachedLabelPixmap", start)
body = source[start:end]

assert "pSD == NULL || menuW == NULL" in body
assert "bitmapIndex < 0" in body
assert "bitmapIndex >= pSD->bitmapCacheCount" in body
assert "pSD->bitmapCache == NULL" in body
assert body.index("pSD == NULL") < body.index("pSD->bitmapCache[bitmapIndex]")

print("PASS MWM cached-label bitmap lookup is bounded")
