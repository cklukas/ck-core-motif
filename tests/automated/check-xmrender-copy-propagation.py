#!/usr/bin/env python3
"""Check that XmRenderT public rendition copy propagates allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text(encoding="utf-8")
start = source.index("_XmRenditionCopy(XmRendition rend")
end = source.index("/* Creates new rendertable", start)
region = source[start:end]
assert "if (shared) toRend = CopyRendition(rend);" in region
assert "if (toRend == NULL)" in region
assert "return(NULL);" in region
assert "if (*toRend != *rend)" in region
print("PASS XmRenderT rendition-copy failure is propagated")
