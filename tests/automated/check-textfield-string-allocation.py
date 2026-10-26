#!/usr/bin/env python3
"""Check text-field string getters validate allocation and conversion bounds."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()

start = source.index("XmTextFieldGetString(Widget w)")
end = source.index("int \nXmTextFieldGetSubstring", start)
string_getter = source[start:end]
assert "temp_str = (char *) XtMalloc((Cardinal)allocation_bytes);" in string_getter
assert "if (temp_str == NULL)" in string_getter
assert "UINT_MAX / (size_t)tf->text.max_char_size" in string_getter
assert "temp_str[ret_val] = '\\0';" in string_getter
assert "size_t ret_val = 0;" in string_getter

start = source.index("XmTextFieldGetStringWcs(Widget w)")
end = source.index("int \nXmTextFieldGetSubstringWcs", start)
wide_getter = source[start:end]
assert "temp_wcs = (wchar_t*) XtMalloc((Cardinal)allocation_bytes);" in wide_getter
assert "if (temp_wcs == NULL)" in wide_getter
assert "UINT_MAX / sizeof(wchar_t)" in wide_getter
assert "temp_wcs[num_wcs] = (wchar_t)0L;" in wide_getter
assert "size_t num_wcs = 0;" in wide_getter

print("PASS text-field string getters validate allocation and termination")
