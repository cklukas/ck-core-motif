#!/usr/bin/env python3
"""Check PushButtonGadget default-shadow erasure handles tiny geometry."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "PushBG.c").read_text()

needle = "if ((int)pb->rectangle.width <= 2 * delta"
assert source.count(needle) >= 2
assert "XmeClearBorder(XtDisplay(pb), XtWindow(pb), x, y, width, height, size);" in source
assert "XmeClearBorder(XtDisplay (pb), XtWindow (XtParent(pb))," in source

print("PushButtonGadget default-shadow geometry is bounded")
