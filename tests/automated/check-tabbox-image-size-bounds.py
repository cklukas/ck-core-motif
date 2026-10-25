#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/TabBox.c").read_text()
start = source.index("XiRotateImage")
end = source.index("\n    if( degree == 0 || degree == 180 )", source.index("data = (char*)", start))
region = source[start:end]
assert "size_t allocation, packed_width" in region
assert "if (width <= 0 || height <= 0 || depth <= 0)" in region
assert "SIZE_MAX" in region
assert "if (allocation > UINT_MAX)" in region
assert "XtMalloc((Cardinal)allocation)" in region
assert "XtMalloc(tmp)" not in region
print("TabBox image allocation bounds present")
