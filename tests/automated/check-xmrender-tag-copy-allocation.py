#!/usr/bin/env python3
"""Check that XmRenderT scratch-tag copying fails atomically."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text(encoding="utf-8")
start = source.index("_XmRenditionCopy(XmRendition rend")
end = source.index("/* Creates new rendertable", start)
region = source[start:end]
assert "_XmRendTags(toRend) = _XmRendTagCount(rend)" in region
assert "if (_XmRendTagCount(rend) != 0 && _XmRendTags(toRend) == NULL)" in region
assert "FreeRendition(toRend);" in region
assert "FreeHandle(toRend);" in region
print("PASS XmRenderT scratch-tag allocation is guarded")
