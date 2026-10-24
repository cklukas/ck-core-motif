#!/usr/bin/env python3
"""Check incoming selection lengths before int-indexed text operations."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "TextSel.c"
text = source.read_text()
assert text.count("> (unsigned long)INT_MAX") == 3
assert "block.length = (unsigned) *length" not in text
assert text.count("block.length = (int) ds->length") == 2
assert "prim_select->success_status" not in text
assert "prim_select->done_status" not in text
assert "prim_select->num_chars = 0" in text
print("text selection import length contract passed")
