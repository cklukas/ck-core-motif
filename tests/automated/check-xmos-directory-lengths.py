#!/usr/bin/env python3
"""Check Xmos directory-entry lengths retain size_t through cache insertion."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xmos.c").read_text()
assert "AddEntryToCache(char *entryName, size_t entryNameLen)" in source
start = source.index("while (TRUE)")
end = source.index("if (fixedMatchPattern)", start)
region = source[start:end]
assert "size_t dirNameLen = 0;" in region
assert "dirNameLen = strlen(dirName);" in region
assert "AddEntryToCache(dirName, dirNameLen)" in region

print("PASS Xmos directory-entry lengths preserve size_t width")
