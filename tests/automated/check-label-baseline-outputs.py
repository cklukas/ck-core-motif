#!/usr/bin/env python3
"""Check that Label and Label gadget baseline outputs are validated."""

from pathlib import Path


root = Path(__file__).parents[2]
needle = "if (wid == NULL || baselines == NULL || line_count == NULL)"
for name in ("Label.c", "LabelG.c"):
    text = (root / "lib" / "Xm" / name).read_text(encoding="utf-8")
    if needle not in text:
        raise SystemExit(f"{name} baseline output guard is missing")

print("Label and Label gadget baseline output guards are present")
