#!/usr/bin/env python3
"""Check XPM grayscale conversion replaces short color strings safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Color.c").read_text(
    encoding="utf-8"
)
start = source.index("HasExpandedHexColor")
helper = source[start:source.index("Pixmap\n_XmConvertToBW", start)]
body = source[source.index("Pixmap\n_XmConvertToBW"):]
assert "HasExpandedHexColor" in helper
assert "strlen(color) != 13" in helper
assert "isxdigit((unsigned char)color[i])" in helper
assert "malloc(14)" in helper
assert 'snprintf(replacement, 14, "#%04x%04x%04x"' in helper
assert "XpmFree(color->c_color);" in helper
assert "color->c_color = replacement;" in helper
assert "ReplaceColorWithGray(&im.colorTable[0], bw)" in body
assert "ReplaceColorWithGray(&im.colorTable[1], bw2)" in body
assert "ReplaceColorWithGray(&im.colorTable[i], bw)" in body
assert "sprintf(im.colorTable" not in body
assert "memcpy(col + 9" not in body
print("PASS XPM grayscale conversion replaces color strings transactionally")
