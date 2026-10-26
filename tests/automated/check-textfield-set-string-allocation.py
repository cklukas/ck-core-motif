#!/usr/bin/env python3
"""Check the wide modify-verify temporary buffer is bounded and checked."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("XmTextFieldSetString(Widget w,")
end = source.index("XmTextFieldSetStringWcs(", start)
function = source[start:end]

assert "value_length + 1 > UINT_MAX / sizeof(wchar_t)" in function
assert "wbuf = (wchar_t*)XtMalloc((Cardinal)wbuf_bytes);" in function
assert "if (wbuf == NULL)" in function
assert "converted_length == (size_t)-1" in function
assert "converted_length > INT_MAX" in function
assert "mod_value = XtMalloc((Cardinal)wbuf_bytes);" in function
assert "if (mod_value == NULL)" in function

print("PASS wide text-field setter checks modify-verify allocation")
