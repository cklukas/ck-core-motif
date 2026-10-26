#!/usr/bin/env python3
"""Check text-field string setting rejects unrepresentable input lengths."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("XmTextFieldSetString(Widget w,")
end = source.index("XmTextFieldSetStringWcs(", start)
function = source[start:end]

assert "if (value_length > INT_MAX)" in function
assert "length = (int)value_length;" in function
assert "byte_length + (size_t)ret_val > INT_MAX" in function
assert "length = (int)byte_length;" in function

print("PASS text-field string setting bounds input lengths")
