#!/usr/bin/env python3
"""Ensure text-source creation checks capacity arithmetic before allocation."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c"
text = source.read_text(encoding="utf-8")
start = text.index("_XmStringSourceCreate")
end = text.index("\nvoid \n_XmStringSourceDestroy", start)
block = text[start:end]

assert block.count("data->maxlength > INT_MAX") >= 2
assert block.count("UINT_MAX / (size_t)char_size") >= 2
assert "UINT_MAX / (size_t)max_char_size" in block
assert "wcstombs(tmp_value, wc_value, allocation_bytes)" in block
assert "num_chars == INT_MAX" in block

print("text-source creation bounds: ok")
