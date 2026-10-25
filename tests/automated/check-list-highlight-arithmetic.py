#!/usr/bin/env python3
"""Check List highlight dimensions keep all insets signed."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text()

assert "(int)lw->list.margin_width +" in source
assert "(int)lw->primitive.shadow_thickness);" in source
assert "(int)lw->list.margin_height +" in source

print("List highlight arithmetic is signed")
