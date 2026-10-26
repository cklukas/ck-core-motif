#!/usr/bin/env python3
"""Ensure SegmentedControl relayout resources request redisplay."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/SegmentedControl.c").read_text()
start = source.index("static Boolean SetValues")
end = source.index("Boolean XmSegmentedControlSetItems", start)
body = source[start:end]

required = (
    "Boolean redraw = False;",
    "old->segmented.allow_clear != control->segmented.allow_clear",
    "old->segmented.rtl != control->segmented.rtl",
    "old->segmented.spacing != control->segmented.spacing",
    "redraw = True;",
    "return redraw;",
)
missing = [item for item in required if item not in body]
if missing:
    raise SystemExit("SegmentedControl SetValues redraw contract missing: " + ", ".join(missing))

print("PASS SegmentedControl relayout changes request redraw")
