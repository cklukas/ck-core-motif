#!/usr/bin/env python3
"""Check recovery from failed initial MWM bitmap-cache allocation."""

from pathlib import Path


source = Path("clients/mwm/WmImage.c").read_text(encoding="utf-8")
start = source.index("int GetBitmapIndex")
end = source.index("} /* END OF FUNCTION GetBitmapIndex", start)
body = source[start:end]

assert "pSD->bitmapCache == NULL && pSD->bitmapCacheCount == 0" in body
assert "pSD->bitmapCacheSize = 0;" in body
assert body.index("bitmapCacheSize = 0") < body.index("if (pSD->bitmapCacheSize == 0)")

print("PASS MWM bitmap-cache allocation can retry")
