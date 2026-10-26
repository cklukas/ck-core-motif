#!/usr/bin/env python3
"""Check icon-directory cache accumulation uses checked size arithmetic."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconFile.c").read_text()
start = source.index("static DtCachedDir \nMakeCachedDirEntry")
end = source.index("static \tDtCachedDirListStruct", start)
function = source[start:end]

assert "size_t\tbufLen, oldBufLen = 0;" in function
assert "bufLen = strlen(currDirect->d_name);" in function
assert "bufLen >= (size_t)MAX_CACHE_DIR_SIZE" in function
assert "oldBufLen > (size_t)MAX_CACHE_DIR_SIZE - bufLen - 1" in function
assert "memcpy(&(stackBuf[oldBufLen])" in function

print("PASS icon-directory cache accumulation bounds")
