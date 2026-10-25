#!/usr/bin/env python3
"""Check that Label gadget margins callbacks validate their inputs."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "LabelG.c"
text = source.read_text(encoding="utf-8")
needle = "if (w == NULL || margins_rec == NULL)"
if needle not in text:
    raise SystemExit("Label margins input guard is missing")

print("Label gadget margins input guard present")
