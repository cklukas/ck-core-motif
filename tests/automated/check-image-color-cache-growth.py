#!/usr/bin/env python3
"""Check ImageCache color-cache growth is transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("static Boolean\nCacheColorPixel")
end = source.index("static int\nGetCacheColor", start)
region = source[start:end]
assert "CachedColor *new_cache" in region
assert "SIZE_MAX / sizeof(*colorCacheList.cache)" in region
assert "if (new_cache == NULL)" in region
assert "return False;" in region
assert "return True;" in region
callers = source[end:]
assert "!CacheColorPixel(display, colormap, colorname, xcolor)" in callers
assert "XFreeColors(display, colormap, &xcolor->pixel" in callers
print("PASS ImageCache color-cache growth handles allocation failure")
