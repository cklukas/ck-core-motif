#!/usr/bin/env python3
"""Check that directory-cache insertion validates its backing path state."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xmos.c").read_text(encoding="utf-8")
start = source.index("AddEntryToCache(char *entryName")
end = source.index("\n  if (numCacheEntries == numCacheAlloc)", start)
region = source[start:end]
assert "entryName == NULL" in region
assert "dirCacheName == NULL" in region
assert "return 0;" in region
print("PASS directory-cache backing state is guarded")
