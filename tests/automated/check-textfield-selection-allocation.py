#!/usr/bin/env python3
"""Check text-field selection extraction validates sizes and allocations."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()

start = source.index("XmTextFieldGetSelection(")
end = source.index("wchar_t *\nXmTextFieldGetSelectionWcs", start)
selection = source[start:end]

assert "value = XtMalloc((Cardinal)allocation_bytes);" in selection
assert "if (value == NULL)" in selection
assert "num_chars + 1 > UINT_MAX / (size_t)tf->text.max_char_size" in selection
assert "tf->text.prim_pos_right < tf->text.prim_pos_left" in selection
assert "if (length >= allocation_bytes)" in selection

start = source.index("XmTextFieldGetSelectionWcs(")
end = source.index("Boolean \nXmTextFieldRemove", start)
selection_wcs = source[start:end]

assert "if (wc_value == NULL)" in selection_wcs
assert "length + 1 > UINT_MAX / sizeof(wchar_t)" in selection_wcs
assert "tf->text.prim_pos_right < tf->text.prim_pos_left" in selection_wcs

print("PASS text-field selection extraction validates allocation bounds")
