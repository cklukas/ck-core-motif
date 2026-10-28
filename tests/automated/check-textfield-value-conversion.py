#!/usr/bin/env python3
"""Check TextField validation publishes only checked converted values."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("{\n  /* if value is wchar_t *")
end = source.index("/*\n * Initialize the s in the text fields instance record.", start)
body = source[start:end]

assert "size_t converted;" in body
assert "converted = mbstowcs" in body
assert "converted == (size_t)-1" in body
assert "size_t ret_val = 0;" in body
assert "ret_val = wcstombs" in body
assert "ret_val >= (size_t)tf->text.size_allocd" in body
assert "TextF_Value(tf)[ret_val] = '\\0'" in body
assert "tf->text.string_length = mbstowcs" not in body
assert "if (ret_val < 0)" not in body

print("PASS TextField value conversion checks")
