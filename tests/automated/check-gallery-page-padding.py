#!/usr/bin/env python3
"""Ensure gallery notebook pages keep content away from their borders."""
from pathlib import Path

source = (Path(__file__).parents[2] / "demos/programs/ckmotif-gallery/gallery.c").read_text(
    encoding="utf-8"
)
assert source.count("XmNmarginWidth, 16") >= 3
assert source.count("XmNmarginHeight, 12") >= 3
assert "XmNshadowThickness, 0" in source
assert "XmNframeShadowThickness, 0" in source
print("PASS gallery notebook pages use consistent inner padding")
