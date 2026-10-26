#!/usr/bin/env python3
"""Check Xmos directory-cache growth and entry allocation failure handling."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xmos.c").read_text()
start = source.index("AddEntryToCache(")
end = source.index("\n/****************************************************************/\nstatic String\nAllocatePathBuffer", start)
region = source[start:end]

assert "XmDirCache new_cache;" in region
assert "if (entryName == NULL" in region
assert "unsigned new_alloc;" in region
assert "new_cache = (XmDirCache)XtRealloc" in region
assert "if (new_cache == NULL)" in region
assert "numCacheAlloc = new_alloc;" in region
assert "if (dirCache[numCacheEntries] == NULL)" in region

print("PASS Xmos directory-cache allocation checks")
