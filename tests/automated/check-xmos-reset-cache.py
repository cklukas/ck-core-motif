#!/usr/bin/env python3
"""Check Xmos cache path allocation failure fallback."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xmos.c").read_text()
start = source.index("static Boolean\nResetCache(")
end = source.index("\nstatic unsigned char\nAddEntryToCache", start)
region = source[start:end]
assert "if (qDirName == NULL)" in region
assert "if (dirCacheName == NULL)" in region
assert "return False;" in region
assert "return True;" in region
assert "if (loadCache && !ResetCache(qualifiedDir))" in source
assert "loadCache = FALSE;" in source

print("PASS Xmos cache reset allocation checks")
