#!/usr/bin/env python3
"""Check the Text substring wide conversion preserves size_t results."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFunc.c").read_text()
start = source.index("_XmTextGetSubstring(")
end = source.index("int\nXmTextGetSubstring", start)
body = source[start:end]

assert "size_t converted;" in body
assert "converted = mbstowcs" in body
assert "converted == (size_t)-1" in body
assert "destpos += (int)converted" in body
assert "num_chars < 0) num_chars = 0" not in body

print("PASS Text substring conversion guards")
