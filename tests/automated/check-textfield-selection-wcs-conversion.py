#!/usr/bin/env python3
"""Check TextField wide selection export uses the conversion result."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("wchar_t *\nXmTextFieldGetSelectionWcs(")
end = source.index("Boolean \nXmTextFieldRemove", start)
body = source[start:end]

assert "size_t return_val = 0;" in body
assert "return_val = mbstowcs" in body
assert "return_val == (size_t)-1" in body
assert "length = return_val;" in body
assert "return_val < 0" not in body

print("PASS TextField wide selection conversion")
