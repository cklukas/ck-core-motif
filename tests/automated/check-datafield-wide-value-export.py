#!/usr/bin/env python3
"""Check wide DataField values validate wcstombs output before publication."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("if (XmTextF_max_char_size(tf) == 1) { /* Need to store data as char* */")
end = source.index("/* The following is a hack", start)
body = source[start:end]

assert "size_t converted = wcstombs" in body
assert "converted == (size_t)-1" in body
assert "converted >= (size_t)XmTextF_size_allocd(tf)" in body
assert "XmTextF_value(tf)[converted] = '\\0'" in body
assert "(void)wcstombs" not in body

print("PASS DataField wide value export checks wcstombs")
