#!/usr/bin/env python3
"""Check that directory-cache reset sizing is bounded before narrowing."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xmos.c").read_text(encoding="utf-8")
start = source.index("ResetCache(char *qDirName)")
end = source.index("static unsigned char\nAddEntryToCache", start)
region = source[start:end]
assert "size_t name_len;" in region
assert "name_len > SIZE_MAX - MAX_USER_NAME_LEN - 1" in region
assert "name_len > UINT_MAX - MAX_USER_NAME_LEN - 1" in region
assert "dirCacheNameLen = (unsigned)name_len;" in region
print("PASS directory-cache reset sizing is bounded")
