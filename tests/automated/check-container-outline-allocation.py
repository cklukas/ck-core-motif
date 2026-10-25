#!/usr/bin/env python3
"""Check Container outline segments are replaced transactionally."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()
start = source.index("LayoutOutlineDetail(\n\tWidget\twid)")
end = source.index("\n/************************************************************************", start + 1)
region = source[start:end]

assert "XSegment *          new_outline_segs = NULL;" in region
assert "new_outline_segs = (XSegment *)XtCalloc" in region
assert "if (n_outline_segs == 0 || new_outline_segs != NULL)" in region
assert "outline_ready = False;" in region
assert region.index("new_outline_segs = (XSegment *)XtCalloc") < region.index("XtFree((char*)cw->container.outline_segs)")
assert "cw->container.outline_segs = new_outline_segs;" in region
print("Container outlines preserve current geometry on allocation failure")
