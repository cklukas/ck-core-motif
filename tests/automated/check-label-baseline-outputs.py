#!/usr/bin/env python3
"""Check that Label gadget baseline outputs are validated."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "LabelG.c"
text = source.read_text(encoding="utf-8")
needle = "if (wid == NULL || baselines == NULL || line_count == NULL)"
if needle not in text:
    raise SystemExit("Label baseline output guard is missing")

print("Label gadget baseline output guard present")
