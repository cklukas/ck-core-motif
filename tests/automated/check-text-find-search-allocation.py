#!/usr/bin/env python3
"""Check Text search conversion allocations are bounded and nullable."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFind.c").read_text()
assert source.count("(size_t)UINT_MAX / (size_t)tw->text.char_size") == 2
assert source.count("(size_t)UINT_MAX / sizeof(wchar_t)") == 2
assert source.count("if (bits16_search_string == NULL)") == 2
assert source.count("if (wchar_t_search_string == NULL)") == 2

print("Text search conversion allocations are bounded")
