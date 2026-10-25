#!/usr/bin/env python3
"""Check that empty-region shrink allocation is transactional."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Region.c").read_text()
start = source.index("if (newReg->numRects < (newReg->size >> 1))")
end = source.index("\n    XtFree ((char *) oldRects);", start)
region = source[start:end]
empty_region = region[region.rfind("\n\telse\n"):]

assert "XmRegionBox *new_rects = (XmRegionBox *)XtMalloc" in empty_region
assert "if (new_rects != NULL)" in empty_region
assert empty_region.index("if (new_rects != NULL)") < empty_region.index("XtFree((char *)newReg->rects)")
assert empty_region.index("XtFree((char *)newReg->rects)") < empty_region.index("newReg->rects = new_rects")
assert "newReg->rects = (XmRegionBox *) XtMalloc" not in empty_region
assert "UINT_MAX / sizeof(XmRegionBox)" in region

print("PASS empty-region shrink preserves storage on allocation failure")
