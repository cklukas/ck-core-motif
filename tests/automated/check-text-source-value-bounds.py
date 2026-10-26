#!/usr/bin/env python3
"""Ensure text-source value extraction bounds copies and conversions."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c"
text = source.read_text(encoding="utf-8")
start = text.index("_XmStringSourceGetValue")
end = text.index("\nvoid \n_XmStringSourceSetValue", start)
block = text[start:end]

assert "(size_t)block.length > allocation - 1 - (size_t)length" in block
assert "ret_pos <= pos" in block
assert "pos != last_pos" in block
assert "return_val == (size_t)-1" in block
assert "XtFree((char *)wc_temp)" in block

print("text-source value extraction bounds: ok")
