#!/usr/bin/env python3
"""Check that compound-string cache creation handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text(encoding="utf-8")
start = source.index("static _XmStringCache\nCacheGet")
end = source.index("XtPointer \n_XmScanningCacheGet", start)
region = source[start:end]
assert region.count("if (!cache)\n              return NULL;") == 2
assert "cache->cache_type = type;" in region
print("PASS XmString cache allocations are guarded")
