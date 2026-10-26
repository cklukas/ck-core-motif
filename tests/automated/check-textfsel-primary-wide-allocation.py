#!/usr/bin/env python3
"""Check TextField primary-selection wide conversion allocation guards."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFSel.c").read_text()
start = source.index("size_t total_len = strlen(total_value);")
end = source.index("XtFree(total_value);", start)
body = source[start:end]

assert "total_len > (size_t)INT_MAX - 1" in body
assert "total_len > (size_t)UINT_MAX / sizeof(wchar_t) - 1" in body
assert "wc_value = NULL;" in body
assert "if (wc_value != NULL)" in body
assert "XtFree((char*)wc_value);" in body

print("TextField primary-selection wide conversion is allocation-safe")
