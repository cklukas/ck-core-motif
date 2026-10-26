#!/usr/bin/env python3
"""Check wide TextField validation warnings tolerate allocation failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("static void \nValidates(XmTextFieldWidget tf)")
end = source.index("\n/*\n * Get the graphics context", start)
body = source[start:end]

assert body.count("err_str = NULL;") >= 2
assert body.count("if (err_str != NULL)") >= 6
assert body.count('"Unsupported wchar"') == 2

print("TextField wide validation warnings tolerate allocation failure")
