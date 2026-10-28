#!/usr/bin/env python3
"""Check XPM compressed-file fallback guards filename-size arithmetic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmRdFToI.c").read_text(
    encoding="utf-8"
)
assert "#include <unistd.h>" in source
start = source.index("size_t len = strlen(filename);")
region = source[start : source.index("if ( !compressfile )", start)]
assert "len > SIZE_MAX - 4" in region
assert "return (XpmNoMemory);" in region
assert "XpmMalloc(len + 4)" in region
print("PASS XPM compressed-file fallback guards filename size")
