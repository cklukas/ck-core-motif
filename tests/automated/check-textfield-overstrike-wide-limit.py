#!/usr/bin/env python3
"""Check the TextField overstrike wide-buffer Cardinal limit."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("tf->text.onthespot->over_len = lastPos - PreCursor(tf);")
end = source.index("if (tf->text.max_char_size == 1)", start)
body = source[start:end]

assert "SIZE_MAX / sizeof(wchar_t)" in body
assert "(size_t)UINT_MAX / sizeof(wchar_t)" in body
assert body.count("tf->text.onthespot->over_len + 1") == 2

print("PASS TextField overstrike wide Cardinal limit")
