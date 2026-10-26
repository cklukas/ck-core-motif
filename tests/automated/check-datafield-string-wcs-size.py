#!/usr/bin/env python3
"""Check DataField wide-string access checks allocation size."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("XmDataFieldGetStringWcs(")
end = source.index("\nXmDataFieldGetSubstringWcs(", start)
body = source[start:end]
assert "size_t string_length;" in body
assert "string_length > SIZE_MAX / sizeof(wchar_t) - 1" in body
assert "string_length + 1 > UINT_MAX / sizeof(wchar_t)" in body
assert "XtMalloc((Cardinal)(sizeof(wchar_t) *" in body
assert "mbstowcs(temp_wcs, XmTextF_value(tf)," in body

print("DataField wide-string access checks allocation size")
