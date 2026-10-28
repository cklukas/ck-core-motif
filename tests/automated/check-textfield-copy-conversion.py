#!/usr/bin/env python3
"""Check TextField copy APIs return actual converted lengths safely."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()

sub_start = source.index("XmTextFieldGetSubstring(")
sub_end = source.index("wchar_t *\nXmTextFieldGetStringWcs", sub_start)
sub = source[sub_start:sub_end]
assert "wcs_ret == (size_t)-1" in sub
assert "n_bytes = (int)wcs_ret" in sub

wcs_start = source.index("XmTextFieldGetSubstringWcs(")
wcs_end = source.index("XmTextPosition \nXmTextFieldGetLastPosition", wcs_start)
wcs = source[wcs_start:wcs_end]
assert "num_wcs == (size_t)-1" in wcs
assert "num_chars = (int)num_wcs" in wcs

setter_start = source.index("XmTextFieldSetStringWcs(Widget w,")
setter = source[setter_start:source.index("static void \nTextFieldReplace", setter_start)]
assert "result >= (size_t)(num_chars + 1)" in setter
assert "tmp[result] = '\\0'" in setter

print("PASS TextField copy conversion bounds")
