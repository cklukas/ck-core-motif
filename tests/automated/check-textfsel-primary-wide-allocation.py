#!/usr/bin/env python3
"""Check TextField primary-selection wide conversion allocation guards."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFSel.c").read_text()
start = source.index("wc_total_length = _XmTextFieldCountCharacters")
end = source.index("if (wc_total_length > 0)", start)
body = source[start:end]

assert "wc_total_length == INT_MAX" in body
assert "UINT_MAX / sizeof(wchar_t) - 1" in body
assert "wc_total_value = (wchar_t*)XtMalloc((Cardinal)" in body
assert "wc_total_value == NULL" in body
assert "wc_total_length = (int)mbstowcs" in body

print("TextField primary-selection wide conversion is allocation-safe")
