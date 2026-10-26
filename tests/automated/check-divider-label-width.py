#!/usr/bin/env python3
"""Check Divider label width avoids Dimension overflow."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/Divider.c"
text = source.read_text(encoding="utf-8")

assert "divider->divider.label && divider->divider.font" in text
assert "unsigned long requested" in text
assert "requested > USHRT_MAX ? USHRT_MAX : requested" in text
print("Divider label width and preferred geometry are bounded")
