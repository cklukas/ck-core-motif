#!/usr/bin/env python3
"""Check DataField wide-string conversion handles invalid multibyte input."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("XmDataFieldGetStringWcs(")
end = source.index("\nXmDataFieldGetSubstringWcs(", start)
body = source[start:end]
assert "converted = mbstowcs(temp_wcs, XmTextF_value(tf)," in body
assert "if (converted == (size_t)-1)" in body
assert "temp_wcs[0] = (wchar_t)0L;" in body

print("DataField wide-string conversion handles invalid input")
