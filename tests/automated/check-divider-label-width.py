#!/usr/bin/env python3
"""Check Divider label width avoids Dimension overflow."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/Divider.c"
text = source.read_text(encoding="utf-8")

assert "divider->divider.label && divider->divider.font" in text
assert "unsigned long requested" in text
assert "requested > USHRT_MAX ? USHRT_MAX : requested" in text
assert "static Dimension divider_thickness_span(Dimension thickness);" in text
assert "divider->core.height = divider_thickness_span" in text
assert "divider->core.width = divider_thickness_span" in text
assert "thickness > (Dimension)(USHRT_MAX - 12)" in text
print("Divider label width and preferred geometry are bounded")
