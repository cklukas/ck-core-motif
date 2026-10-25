#!/usr/bin/env python3
"""Check bounded XmProgressRing geometry calculations."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ProgressRing.c").read_text(encoding="utf-8")
assert "static Dimension bounded_dimension" in source
assert "bounded_dimension((unsigned long)diameter + label_width" in source
assert "reply->height = bounded_dimension" in source
print("PASS progress-ring geometry is bounded")
