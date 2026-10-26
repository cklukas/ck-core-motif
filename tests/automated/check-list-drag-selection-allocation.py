#!/usr/bin/env python3
"""Check XmList drag selection allocation respects Xt's Cardinal size."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text()
start = source.index("/* CR 8878: selectedItems may contain extraneous items. */")
body = source[start:source.index("ListDragConv->num_strings", start)]
assert "SIZE_MAX / sizeof(XmString)" in body
assert "UINT_MAX / sizeof(XmString)" in body
print("PASS XmList drag selection allocation bounds")
