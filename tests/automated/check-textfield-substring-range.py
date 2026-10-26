#!/usr/bin/env python3
"""Check substring extraction does not narrow large text positions unsafely."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()

start = source.index("XmTextFieldGetSubstring(")
end = source.index("wchar_t *\nXmTextFieldGetStringWcs", start)
substring = source[start:end]
assert "XmTextPosition available;" in substring
assert "if (available > INT_MAX)" in substring
assert "num_chars = INT_MAX;" in substring
assert "num_chars = (int)available;" in substring

start = source.index("XmTextFieldGetSubstringWcs(")
end = source.index("XmTextPosition \nXmTextFieldGetLastPosition", start)
substring_wcs = source[start:end]
assert "XmTextPosition available;" in substring_wcs
assert "if (available > INT_MAX)" in substring_wcs
assert "num_chars = INT_MAX;" in substring_wcs
assert "num_chars = (int)available;" in substring_wcs

print("PASS text-field substring ranges avoid position narrowing")
