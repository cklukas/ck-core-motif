#!/usr/bin/env python3
"""Check I18List pixmap conversion refuses unavailable allocations."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text()
start = source.index("static void\nListConvert")
end = source.index("\n/*ARGSUSED*/\nstatic void\nListPreDestProc", start)
region = source[start:end]

assert "if (ListDragConv == NULL)" in region
assert "cs->status = XmCONVERT_REFUSE;" in region
pix = region.index("pix = (Pixmap *) XtMalloc(sizeof(Pixmap));")
assert "if (pix == NULL)" in region[pix:]
assert region.index("if (pix == NULL)", pix) < region.index("*pix = ListDragConv->pixmap", pix)
print("I18List pixmap conversion handles allocation failure")
