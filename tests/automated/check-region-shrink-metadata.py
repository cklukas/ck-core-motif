#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/Region.c").read_text()
start = source.index("if (newReg->numRects < (newReg->size >> 1))")
end = source.index("XtFree ((char *) oldRects);", start)
body = source[start:end]
assert "int prev_size = newReg->size;" in body
assert "XmRegionBox *new_rects;" in body
assert "if (new_rects != NULL)" in body
assert "newReg->size = prev_size;" in body
print("Region shrink metadata guards present")
