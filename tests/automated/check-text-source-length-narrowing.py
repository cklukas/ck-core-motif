#!/usr/bin/env python3
"""Ensure Text source creation proves lengths before narrowing them."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text(
    encoding="utf-8"
)
start = source.index("_XmStringSourceCreate(")
end = source.index("\nBoolean \n_XmStringSourceHasSelection", start)
region = source[start:end]

assert "if (MB_CUR_MAX > (size_t)INT_MAX)" in region
assert "converted_bytes = value == NULL ? 0" in region
assert "converted_bytes == (size_t)-1" in region
assert "value_bytes > (size_t)INT_MAX" in region
assert "_XmTextCountCharacters(value, (int)value_bytes)" in region
assert "_XmTextCountCharacters(value, strlen(value))" not in region

print("Text source creation checks size_t lengths before int conversion")
