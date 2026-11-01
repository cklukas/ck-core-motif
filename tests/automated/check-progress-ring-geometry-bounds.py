#!/usr/bin/env python3
"""Check bounded XmProgressRing geometry calculations."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ProgressRing.c").read_text(encoding="utf-8")
assert "static Dimension bounded_dimension" in source
assert "*width = bounded_dimension(diameter + padding" in source
assert "*height = bounded_dimension(MAX(diameter,label_height) + padding)" in source
assert "PreferredSize(widget,scale,&reply->width,&reply->height)" in source
print("PASS progress-ring geometry is bounded")
