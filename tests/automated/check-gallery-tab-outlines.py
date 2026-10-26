#!/usr/bin/env python3
"""Check that every gallery view tab has a quiet inactive outline."""

from pathlib import Path


source = Path(__file__).parents[2] / "demos" / "programs" / "ckmotif-gallery" / "gallery.c"
text = source.read_text(encoding="utf-8")

if "XAllocNamedColor" not in text or '"#aeb7c2"' not in text:
    raise SystemExit("gallery tab outline color is not defined")
if text.count("style_gallery_tab(state.") != 3:
    raise SystemExit("not all gallery view tabs receive an outline")
if "XmNborderWidth, 0, XmNshadowThickness, 1" not in text:
    raise SystemExit("gallery tabs do not use a one-pixel Motif shadow outline")
if "XmNtopShadowColor, outline, XmNbottomShadowColor, outline" not in text:
    raise SystemExit("gallery tabs do not use a uniform gray outline")

print("gallery view tabs have gray inactive outlines")
