#!/usr/bin/env python3
"""Check DataField validation rejects unrepresentable source lengths."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("df_ValidateString(")
end = source.index("\n/* The following is a hack", start)
body = source[start:end]
assert "input_length > (size_t)INT_MAX" in body
assert "str_len == INT_MAX" in body
assert "(size_t)str_len + 1 > (size_t)UINT_MAX / sizeof(wchar_t)" in body
assert "(size_t)str_len > (size_t)UINT_MAX / sizeof(wchar_t) - 30" in body

print("DataField validation rejects unrepresentable source lengths")
