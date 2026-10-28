#!/usr/bin/env python3
"""Check image-cache capacity is committed only after realloc succeeds."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("static Boolean\nCacheColorPixel")
end = source.index("static int\nGetCacheColor", start)
body = source[start:end]

assert "int new_max_entries;" in body
assert "new_max_entries = colorCacheList.maxEntries + 25;" in body
assert "if (new_cache == NULL)" in body
assert body.index("if (new_cache == NULL)") < body.index("colorCacheList.maxEntries = new_max_entries;")
assert "colorCacheList.maxEntries += 25" not in body
print("image-cache growth publishes capacity only after realloc succeeds")
