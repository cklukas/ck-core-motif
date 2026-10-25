#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/FontS.c").read_text()
start = source.index("left_len = strlen(left_buf);")
end = source.index("XtFree(right_buf);", start)
region = source[start:end]
assert "right_len > SIZE_MAX - 6" in region
assert "left_len > SIZE_MAX - right_len - 6" in region
assert "> UINT_MAX" in region
assert "XtMalloc((Cardinal)fbuf_size)" in region
assert "snprintf(fbuf, fbuf_size" in region
print("font fallback size checks present")
