#!/usr/bin/env python3
"""Check Gadget unit/direction trait getters validate widget inputs."""

from pathlib import Path


source = Path("lib/Xm/Gadget.c").read_text(encoding="utf-8")
start = source.rindex("GetUnitType(Widget w)")
end = source.index("\n}", start)
unit = source[start:end]
assert "w == NULL || !XtIsSubclass(w, xmGadgetClass)" in unit
assert "return XmPIXELS" in unit

start = source.rindex("GetDirection(Widget w)")
end = source.index("\n}", start)
direction = source[start:end]
assert "w == NULL || !XtIsSubclass(w, xmGadgetClass)" in direction
assert "return XmDEFAULT_DIRECTION" in direction

print("Gadget unit/direction trait guards present")
