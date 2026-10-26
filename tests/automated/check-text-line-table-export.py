#!/usr/bin/env python3
"""Ensure Text line-table export validates count and allocation."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Text.c"
text = source.read_text(encoding="utf-8")
start = text.index("_XmTextGetLineTable(Widget widget,")
end = text.index("\n/********************************************", start)
block = text[start:end]

assert "*total_lines <= 0" in block
assert "SIZE_MAX / sizeof(XmTextLineTableRec)" in block
assert "UINT_MAX / sizeof(XmTextLineTableRec)" in block
assert "XtMalloc(" in block
assert "if (line_table == NULL)" in block
assert "*total_lines = 0;" in block

print("text line-table export allocation guard: ok")
