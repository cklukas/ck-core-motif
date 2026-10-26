#!/usr/bin/env python3
"""Check XPM color serialization handles snprintf truncation safely."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrDatFrI.c").read_text()
start = source.index("static int\nCreateColors(")
end = source.index("static int\nCreatePixels(", start)
function = source[start:end]

assert "size_t available;" in function
assert "int written;" in function
assert "if (used >= sizeof(buf))" in function
assert "written = snprintf(s, available" in function
assert "written < 0 || (size_t)written >= available" in function
assert "s += written;" in function
assert "s += strlen(s);" not in function

print("PASS XPM color serialization bounds snprintf output")
