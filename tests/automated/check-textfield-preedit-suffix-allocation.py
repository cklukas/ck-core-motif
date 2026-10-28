#!/usr/bin/env python3
"""Check overstrike preedit suffix allocation bounds."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("rest_len = (XmTextPosition)(PreEnd(tf) - PreStart(tf)")
end = source.index("    if (tf->text.overstrike)", start)
body = source[start:end]

assert "rest_len < 0" in body
assert "(size_t)rest_len > SIZE_MAX / sizeof(wchar_t) - 1" in body
assert "(size_t)rest_len + 1 > UINT_MAX / sizeof(wchar_t)" in body
assert "rest_bytes = ((size_t)rest_len + 1) * sizeof(wchar_t)" in body
assert "XtMalloc(rest_len+1)" not in body
assert "XtMalloc((rest_len+1)*sizeof(wchar_t))" not in body

print("PASS TextField preedit suffix allocation guards")
