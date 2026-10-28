#!/usr/bin/env python3
"""Check TextField input-method reset conversions use checked results."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("static void\nTextFieldResetIC(Widget w)")
end = source.index("static void \nResetUnder", start)
body = source[start:end]

assert "size_t num_chars, mb_length;" in body
assert "size_t num_bytes;" in body
assert "num_bytes == (size_t)-1 || num_bytes >= byte_capacity" in body
assert "num_chars == (size_t)-1 || num_chars > (size_t)INT_MAX" in body
assert "num_chars < 0" not in body
assert "(int)num_chars" in body

print("PASS TextField input-method conversion guards")
