#!/usr/bin/env python3
"""Check empty and bounded locale text-property import."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c"
text = source.read_text()
assert "text_prop->nitems == 0" in text
assert "XmStringGenerate((XtPointer)\"\", tag, type, NULL)" in text
assert text.count("i + 1 < (int)text_prop->nitems") == 2
assert "text_prop->nitems - 1" not in text
print("empty text property import contract passed")
