#!/usr/bin/env python3
"""Check that shared XmRenderT rendition handle allocation is guarded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text(encoding="utf-8")
start = source.index("CopyRendition(XmRendition rend)\n{")
end = source.index("/* Make a copy of a rendition", start)
region = source[start:end]
assert "copy = GetHandle(_XmRendition);" in region
assert "if (copy == NULL)" in region
assert "_XmRendRefcountDec(rend);" in region
assert "SetPtr(copy, GetPtr(rend));" in region
print("PASS shared XmRenderT handle allocation is guarded")
