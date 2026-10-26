#!/usr/bin/env python3
"""Check TextField SetValues empty wide-value allocation."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("static Boolean \nSetValues(Widget old")
end = source.index("\nstatic void ", start)
body = source[start:end]

assert "wchar_t *empty_value = (wchar_t*)XtMalloc(sizeof(wchar_t));" in body
assert "if (empty_value == NULL)" in body
assert "_XmTextFieldDrawInsertionPoint(new_tf, True);" in body
assert "empty_value[0] = (wchar_t)0;" in body

print("TextField SetValues checks empty wide-value allocation")
