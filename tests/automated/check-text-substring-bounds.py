#!/usr/bin/env python3
"""Check generic Text substring validation and element-sized bounds."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFunc.c").read_text()
start = source.index("static int\n_XmTextGetSubstring")
end = source.index("\nint\nXmTextGetSubstring", start)
body = source[start:end]

assert "buffer == NULL || start < 0 || num_chars < 0 || buf_size <= 0" in body
assert "start > (XmTextPosition)LONG_MAX - (XmTextPosition)num_chars" in body
assert "(size_t)destpos + (size_t)block.length + 1 > (size_t)buf_size" in body
assert "(size_t)destpos + (size_t)num_chars + 1 > (size_t)buf_size" in body
assert "block.length < 0" in body
assert "num_chars < 0" in body

print("Generic Text substring bounds are validated")
