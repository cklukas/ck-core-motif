#!/usr/bin/env python3
"""Check bounded color-selector label formatting."""
from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorS.c").read_text(encoding="utf-8")
region = text
assert region.count("snprintf(new_label, sizeof(new_label)") == 3
assert "sprintf(new_label" not in region
print("ColorSelector label formatting is bounded")
