#!/usr/bin/env python3
"""Check that ScrolledWindow auto-drag allocation failure is balanced."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScrolledW.c").read_text(encoding="utf-8")
start = source.index("static void\nHandleDrag(")
end = source.index("/*ARGSUSED*/\nstatic void", start)
region = source[start:end]
assert "XtMalloc(sizeof(AutoDragClosure))" in region
assert "if (auto_clos == NULL)" in region
assert "sw->swindow.auto_drag_closure = NULL;" in region
assert "XmDropSiteEndUpdate(w);" in region
print("PASS ScrolledWindow auto-drag allocation is guarded")
