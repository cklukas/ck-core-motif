#!/usr/bin/env python3
"""Check DataField validation capacity arithmetic stays representable."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("df_ValidateString(")
end = source.index("\n/* The following is a hack", start)
body = source[start:end]
assert body.count("(size_t)INT_MAX - 30") >= 2
assert "SIZE_MAX / sizeof(wchar_t) - 30" in body
assert "(size_t)UINT_MAX / sizeof(wchar_t) - 30" in body
assert "XmTextF_size_allocd(tf) = (str_len + 30) * sizeof(wchar_t);" in body

print("DataField validation capacity arithmetic stays representable")
