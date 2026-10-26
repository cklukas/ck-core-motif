from pathlib import Path

source = Path("clients/mwm/WmImage.c").read_text(encoding="utf-8")
start = source.index("GetBitmapIndex")
end = source.index("END OF FUNCTION GetBitmapIndex", start)
body = source[start:end]

assert "BitmapCache *initial_cache;" in body
assert "initial_cache = (BitmapCache *) XtMalloc" in body
assert "if (initial_cache == NULL)" in body
assert body.index("pSD->bitmapCache = initial_cache") > body.index(
    "if (initial_cache == NULL)"
)
assert body.index("pSD->bitmapCacheSize = BITMAP_CACHE_INC") > body.index(
    "pSD->bitmapCache = initial_cache"
)
print("PASS MWM bitmap-cache initialization remains retryable after allocation failure")
