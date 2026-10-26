#!/usr/bin/env python3
"""Check TextField wide-value validation before conversion."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("static void \nValidates(XmTextFieldWidget tf)")
end = source.index("\n/*\n * Get the graphics context", start)
body = source[start:end]

assert "TextF_WcValue(tf) = (wchar_t*)XtMalloc" in body
assert "if (TextF_WcValue(tf) == NULL)" in body
assert "XmStackFree(start_temp, stack_cache);" in body
assert "tf->text.string_length = mbstowcs" in body

print("TextField wide-value validation checks allocation")
