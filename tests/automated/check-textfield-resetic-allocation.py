#!/usr/bin/env python3
"""Check TextField preedit reset allocation bounds."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("TextFieldResetIC(Widget w)")
end = source.index("XmImMbResetIC(w, &mb);", start)
body = source[start:end]

assert "PreEnd(tf) < PreStart(tf)" in body
assert "preedit_length > SIZE_MAX / sizeof(wchar_t) - 1" in body
assert "preedit_length + 1 > UINT_MAX / sizeof(wchar_t)" in body
assert "preedit_bytes = (preedit_length + 1) * sizeof(wchar_t)" in body
assert "XtMalloc((PreEnd(tf) - PreStart(tf)+1)*sizeof(wchar_t))" not in body

print("PASS TextField reset preedit allocation guards")
