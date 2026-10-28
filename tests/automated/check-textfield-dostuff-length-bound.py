#!/usr/bin/env python3
"""Check TextField locale-expanded transfers reject an unrepresentable int."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFSel.c").read_text(
    encoding="utf-8"
)
start = source.index("if (tf->text.max_char_size == 1) {", source.index("DoStuff"))
body = source[start : source.index("\n\t  } else {", start)]
assert "size_t total_len = strlen(total_value);" in body
assert "total_len > (size_t)INT_MAX" in body
assert "prim_select->num_chars = (int)total_len;" in body
assert body.index("total_len > (size_t)INT_MAX") < body.index(
    "prim_select->num_chars = (int)total_len"
)
print("PASS TextField locale-expanded transfer bounds strlen before int conversion")
