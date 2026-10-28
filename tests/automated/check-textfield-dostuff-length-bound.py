#!/usr/bin/env python3
"""Check TextField locale-expanded transfers reject an unrepresentable int."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFSel.c").read_text(
    encoding="utf-8"
)
start = source.index("if (tf->text.max_char_size == 1) {", source.index("DoStuff"))
body = source[start : source.index("\n      } else { /* must convert", start)]
assert "size_t len = strlen(total_value);" in body
assert "len > (size_t)INT_MAX" in body
assert "num_chars = (int)len;" in body
assert body.index("len > (size_t)INT_MAX") < body.index(
    "num_chars = (int)len"
)
print("PASS TextField locale-expanded transfer bounds strlen before int conversion")
