#!/usr/bin/env python3
"""Check text-field replacement conversion buffers and length bounds."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("static void \nTextFieldReplace(")
end = source.index("void \nXmTextFieldReplace(", start)
function = source[start:end]

assert "wide_length == INT_MAX" in function
assert "wide_length + 1 > SIZE_MAX / (size_t)tf->text.max_char_size" in function
assert "value = XtMalloc((Cardinal)conversion_bytes);" in function
assert "if (value == NULL)" in function
assert "converted_length == (size_t)-1" in function
assert "input_length > INT_MAX" in function
assert "input_length + 1 > SIZE_MAX / sizeof(wchar_t)" in function
assert "wc_value = (wchar_t *) XtMalloc((Cardinal)conversion_bytes);" in function
assert "if (wc_value == NULL)" in function
assert "wc_value[0] = (wchar_t) 0L;" in function
conversion_start = function.index("converted_length = wcstombs")
conversion_end = function.index("rep_result =", conversion_start)
assert 'value = ""' not in function[conversion_start:conversion_end]

print("PASS text-field replacement conversion checks")
