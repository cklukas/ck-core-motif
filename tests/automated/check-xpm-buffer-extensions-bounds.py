#!/usr/bin/env python3
"""Check XPM buffer extension serialization handles truncation safely."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrBufFrI.c").read_text()
start = source.index("static int\nWriteExtensions(")
end = source.index("static int\nCommentsSize(", start)
function = source[start:end]

assert "if (dataptr == NULL || used_size == NULL || ext == NULL" in function
assert "written = snprintf(s, available" in function
assert "written < 0 || (size_t)written >= available" in function
assert "if (a != 0 && ext->lines == NULL)" in function
assert "if (*line == NULL)" in function
assert "memcpy(s, \"\\n\\\"XPMENDEXT\\\"\"" not in function
assert "return 1;" in function
assert "!WriteExtensions" in source

print("PASS XPM buffer extension serialization bounds output")
