#!/usr/bin/env python3
"""Check that Sash inherits a cursor when crosshair creation fails."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Sash.c"
text = source.read_text(encoding="utf-8")
start = text.index("static void \nRealize(")
end = text.index("\n/*ARGSUSED*/\nstatic void \nSashDisplayDestroyCallback", start)
realize = text[start:end]

assert "SashCursor = XCreateFontCursor" in realize
assert "if (SashCursor != None)" in realize
assert "*p_valueMask |= CWCursor;" in realize
assert "XtCreateWindow (w, InputOutput, CopyFromParent, *p_valueMask, attributes);" in realize
allocation = realize.index("SashCursor = XCreateFontCursor")
created = realize.index("if (SashCursor != None)", allocation)
cache = realize.index("->SashCursor =", created)
callback = realize.index("XtAddCallback((Widget)dd", cache)
assert created < cache < callback

print("Sash cursor allocation failure preserves cursor inheritance")
