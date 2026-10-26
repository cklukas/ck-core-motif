#!/usr/bin/env python3
"""Check wide-character byte counting cannot wrap its int result."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("_XmTextFieldCountBytes(")
end = source.index("/*ARGSUSED*/", start)
function = source[start:end]

assert "if (n_bytes > INT_MAX - n_bytes_per_char)" in function
assert "return INT_MAX;" in function

print("PASS text-field byte counting saturates at INT_MAX")
