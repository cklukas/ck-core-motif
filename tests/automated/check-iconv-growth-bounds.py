#!/usr/bin/env python3
"""Check overflow-safe iconv output growth."""
from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "ResEncod.c").read_text(encoding="utf-8")
start = text.index("\nConvertWithIconv(const char")
end = text.index("#endif", start)
region = text[start:end]
assert "if (len == UINT_MAX)" in region
assert "outbuf_size = (size_t)len + 1" in region
assert "outbuf_size > SIZE_MAX / 2" in region
assert "XtFree(dest);" in region
assert "if (len < 0)" not in region
print("iconv output growth is overflow-safe")
