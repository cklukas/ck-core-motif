#!/usr/bin/env python3
"""Check inconsistent MWM bitmap-cache metadata handling."""

from pathlib import Path


source = Path("clients/mwm/WmImage.c").read_text(encoding="utf-8")
start = source.index("int GetBitmapIndex")
end = source.index("} /* END OF FUNCTION GetBitmapIndex", start)
body = source[start:end]

assert "pSD->bitmapCacheCount != 0 && pSD->bitmapCache == NULL" in body
assert body.index("bitmapCache == NULL") < body.index("bitmapc = pSD->bitmapCache")

print("PASS MWM bitmap-cache metadata is validated")
