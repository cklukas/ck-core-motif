#!/usr/bin/env python3
"""Ensure TextField modify-verify input allocation is bounded."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c"
text = source.read_text(encoding="utf-8")
start = text.index("newblock.format = XmFMT_8_BIT;")
end = text.index("\n  /* Fill in the appropriate structs */", start)
block = text[start:end]

assert "*insert_length < 0" in block
assert "SIZE_MAX /" in block
assert "initial_bytes > (size_t)INT_MAX" in block
assert "UINT_MAX" in block
assert "if (newblock.ptr == NULL)" in block
assert "return(False);" in block

print("TextField modify-verify allocation guard: ok")
