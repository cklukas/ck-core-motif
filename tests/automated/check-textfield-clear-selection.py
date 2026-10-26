#!/usr/bin/env python3
"""Check TextField clear-selection wide-buffer allocation."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("static void \nClearSelection")
end = source.index("\nstatic void", start + 20)
body = source[start:end]

assert "wc_spaces = (wchar_t *)XtMalloc" in body
assert "if (wc_spaces == NULL)" in body
assert "_XmTextFieldDrawInsertionPoint(tf, True);" in body

print("TextField clear-selection wide allocation is checked")
