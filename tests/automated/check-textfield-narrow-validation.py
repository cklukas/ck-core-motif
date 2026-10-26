#!/usr/bin/env python3
"""Check TextField narrow-value validation before wcstombs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("static void \nValidates(XmTextFieldWidget tf)")
end = source.index("\n/*\n * Get the graphics context", start)
body = source[start:end]

assert "TextF_Value(tf) = XtMalloc" in body
assert "if (TextF_Value(tf) == NULL)" in body
assert "tf->text.string_length = 0;" in body
assert "ret_val = wcstombs(TextF_Value(tf)" in body

print("TextField narrow-value validation checks allocation")
