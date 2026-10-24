#!/usr/bin/env python3
"""Check that FontSelector numeric XLFD fields are parsed with bounds."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FontS.c").read_text()

assert "static short\nParseFontShort" in source
assert "value < SHRT_MIN || value > SHRT_MAX" in source
assert "current->point_size = ParseFontShort(temp);" in source
assert "current->resolution_x = ParseFontShort(temp);" in source
assert "current->resolution_y = ParseFontShort(temp);" in source
assert "value * POINT_DIVIDE" in source

fill_data = source[source.index("static Boolean\nFillData") : source.index("#undef GET_NEXT_FIELD")]
assert "atoi(" not in fill_data
size_changed = source[source.index("static void\nSizeChanged") : source.index("static void\nChangeEncoding")]
assert "atoi(" not in size_changed
print("FontSelector XLFD numeric fields use checked conversion")
