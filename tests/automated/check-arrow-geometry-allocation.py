#!/usr/bin/env python3
"""Check that legacy arrow geometry allocation is transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Obso1_2.c").read_text(encoding="utf-8")
start = source.index("void _XmGetArrowDrawRects (")
end = source.index("static void", start)
region = source[start:end]

assert "new_top = (XRectangle *) XtMalloc" in region
assert "new_cent = (XRectangle *) XtMalloc" in region
assert "new_bot = (XRectangle *) XtMalloc" in region
assert "if (new_top == NULL || new_cent == NULL || new_bot == NULL)" in region
assert region.count("XtFree((char *)new_") == 3
assert "if (top_count == NULL || cent_count == NULL || bot_count == NULL" in region
print("PASS legacy arrow geometry allocation is guarded")
