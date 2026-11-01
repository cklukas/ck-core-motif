#!/usr/bin/env python3
"""Keep ToggleSwitch on the shared, native-pixel-qualified rounded renderer.

Pixel symmetry and occupied bounds are tested by run-modern-rounding.py;
this guard prevents the switch from silently reintroducing a private arc path.
"""
from pathlib import Path
import re

source = (Path(__file__).parents[2] / "lib/Xm/ToggleSwitch.c").read_text(
    encoding="utf-8"
)
start = source.index("static void fill_round_rect")
end = source.index("static void Initialize", start)
context = source[start:end]

if "_XmAppearanceFillRoundedRectangle(" not in context:
    raise SystemExit("ToggleSwitch must use the shared rounded-surface renderer")
if re.search(r"\bX(?:Fill|Draw)Arcs?\s*\(", source):
    raise SystemExit("ToggleSwitch must not reintroduce a private X arc path")
if "width > INT_MAX" not in context or "height > INT_MAX" not in context:
    raise SystemExit("unsigned dimensions must be checked before signed conversion")

print("ToggleSwitch shared rounded-drawing check passed")
