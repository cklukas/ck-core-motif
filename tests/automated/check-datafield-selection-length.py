#!/usr/bin/env python3
"""Check DataField selection conversion bounds external byte lengths."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("if (XmTextF_max_char_size(tf) == 1){", source.index("df_DoStuff"))
end = source.index("prim_select->num_chars = mbstowcs", start)
body = source[start:end]
assert "*length <= (unsigned long)INT_MAX" in body
assert "prim_select->num_chars = (int) *length" in body

print("DataField selection conversion bounds external byte lengths")
