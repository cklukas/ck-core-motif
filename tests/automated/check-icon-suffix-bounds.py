#!/usr/bin/env python3
"""Check sized icon-name construction uses checked size_t arithmetic."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "IconFile.c"
text = source.read_text(encoding="utf-8")
start = text.index("/* need to add size suffix if size is specified */")
end = text.index("\n       /*\n        * try to see if its already", start)
region = text[start:end]
assert "size_t basenameLen" in region
assert "size_t sizeLen" in region
assert "size_t extLen" in region
assert "sizeLen > SIZE_MAX - basenameLen - 1" in region
assert "extLen > UINT_MAX" in region
assert "XtMalloc((Cardinal)extLen)" in region
assert "if (ext_name == NULL)" in region
print("sized icon-name construction bounds lengths and allocation")
