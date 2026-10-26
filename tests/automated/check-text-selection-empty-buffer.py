#!/usr/bin/env python3
"""Ensure failed text conversion handles its empty fallback buffer safely."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TextSel.c"
text = source.read_text(encoding="utf-8")
start = text.index("block.ptr = total_value = XtMalloc((unsigned)1);")
end = text.index("\n      } else {", start)
block = text[start:end]

assert "if (block.ptr == NULL)" in block
assert "XtFree((char *)ds->value);" in block
assert "prim_select->num_chars = 0;" in block
assert "*(block.ptr) = '\\0';" in block

print("text selection empty-buffer allocation guard: ok")
