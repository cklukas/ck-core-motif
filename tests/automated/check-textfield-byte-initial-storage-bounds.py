#!/usr/bin/env python3
"""Check TextField byte initial storage validates strlen before narrowing."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("if (tf->text.max_char_size == 1) {", source.index("*temp_str = '\\0'"))
end = source.index("} else { /* Need wchar_t* data", start)
body = source[start:end]

assert "input_length = strlen(start_temp)" in body
assert "input_length > (size_t)INT_MAX - 30" in body
assert "input_length > (size_t)UINT_MAX - 30" in body
assert "allocation_bytes = input_length + 30" in body
assert "(Cardinal)allocation_bytes" in body
assert "tf->text.string_length = strlen(start_temp)" not in body
print("TextField byte initial storage is bounds-checked")
