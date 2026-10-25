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
assert "const char *prompt =" in source
assert "candidate_extents.width" in source
assert 'strtok(remaining, " ")' in source
assert "create_gallery_tab" in source
assert "xmFrameWidgetClass" in source
assert '"#aeb7c2"' in source
print("PASS gallery notebook pages use consistent inner padding")
