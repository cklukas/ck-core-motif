#!/usr/bin/env python3
"""Check ColorObj property fields use checked hexadecimal parsing."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorObj.c").read_text(encoding="utf-8")
start = source.index("ParseColorHexField")
end = source.index("static Boolean\nFetchPixelData", start)
helper = source[start:end]
fetch = source[end:source.index("static Boolean\nColorCachePropertyExists", end)]

assert "strtoul(*cursor, &end, 16)" in helper
assert "errno == ERANGE" in helper
assert "*end != '_'" in helper
assert "sscanf" not in fetch
assert "parsed_color_use > (unsigned long)INT_MAX" in fetch
assert "ParseColorHexField(&cursor, &(colors[i].sc))" in fetch
print("PASS ColorObj property hexadecimal parsing is checked")
