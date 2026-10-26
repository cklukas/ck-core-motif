#!/usr/bin/env python3
"""Ensure IconG baseline queries handle array allocation failure."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "IconG.c"
text = source.read_text(encoding="utf-8")
start = text.index("\nGetBaselines(\n\tWidget") + 1
end = text.index("\n    GetLabelXY", start)
block = text[start:end]

assert "*line_count = 0;" in block
assert "*baselines = NULL;" in block
assert "base_array = (Dimension *)XtMalloc(sizeof(Dimension));" in block
assert "if (base_array == NULL)" in block
assert "return False;" in block
assert "*line_count = 1;" in block

print("IconG baseline allocation guard: ok")
