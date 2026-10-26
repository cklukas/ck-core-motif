#!/usr/bin/env python3
"""Ensure Text reflow scratch-table allocation is checked."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Text.c"
text = source.read_text(encoding="utf-8")
start = text.index("/* do initial allocation of the temp_table */")
end = text.index("_XmTextRealignLineTable", start)
block = text[start:end]

assert "temp_table_size < 0" in block
assert "SIZE_MAX / sizeof(XmTextLineTableRec)" in block
assert "UINT_MAX / sizeof(XmTextLineTableRec)" in block
assert "temp_table = (XmTextLineTable) XtMalloc" in block
assert "if (temp_table == NULL)" in block

print("text reflow scratch-table allocation guard: ok")
