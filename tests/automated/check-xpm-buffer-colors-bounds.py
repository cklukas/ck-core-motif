#!/usr/bin/env python3
"""Check XPM buffer color serialization handles snprintf truncation safely."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrBufFrI.c").read_text()
start = source.index("static int\nWriteColors(")
end = source.index("static int\nWritePixels(", start)
function = source[start:end]

assert "size_t available;" in function
assert "int written;" in function
assert "if (used >= sizeof(buf))" in function
assert "written = snprintf(s, available" in function
assert "written < 0 || (size_t)written >= available" in function
assert "s += written;" in function
assert "s += strlen(s);" not in function

print("PASS XPM buffer color serialization bounds snprintf output")
