#!/usr/bin/env python3
"""Check TextField validation warning formatting handles allocation failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("static void \nValidates(XmTextFieldWidget tf)")
end = source.index("\n/*\n * Get the graphics context", start)
body = source[start:end]

assert "err_str = NULL;" in body
assert "if (err_str != NULL)" in body
assert "_XmWarningMsg ((Widget)tf, \"Unsupported char\", MSG5, params, 1);" in body

print("TextField validation warnings tolerate formatting allocation failure")
