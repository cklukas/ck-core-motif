#!/usr/bin/env python3
"""Check ToggleSwitch uses symmetric X11 rounded-shape primitives."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/ToggleSwitch.c").read_text(
    encoding="utf-8"
)
start = source.index("static void fill_round_rect")
end = source.index("static void Initialize", start)
context = source[start:end]

if context.count("XFillArc") != 2:
    raise SystemExit("rounded control must draw two symmetric filled arcs")
if "while (" in context or "XFillRectangle(display, window, gc, left" in context:
    raise SystemExit("rounded control still uses hand-rasterized edge rows")
if "body_width = width - diameter;" not in context:
    raise SystemExit("rounded control must derive its center span from dimensions")

print("ToggleSwitch rounded-drawing check passed")
