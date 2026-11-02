#!/usr/bin/env python3
"""Check that Label geometry-trait outputs are validated."""

from pathlib import Path


root = Path(__file__).parents[2]
needle = "if (wid == NULL || baselines == NULL || line_count == NULL)"
for name in ("Label.c", "LabelG.c"):
    text = (root / "lib" / "Xm" / name).read_text(encoding="utf-8")
    if needle not in text:
        raise SystemExit(f"{name} baseline output guard is missing")
    display_start = text.index(
        "static Boolean\nXmLabelGetDisplayRect("
        if name == "Label.c"
        else "static Boolean\nXmLabelGadgetGetDisplayRect("
    )
    display_region = text[display_start:text.index("\n}", display_start)]
    if "if (w == NULL || displayrect == NULL)" not in display_region:
        raise SystemExit(f"{name} display-rectangle output guard is missing")

print("Label geometry-trait output guards are present")
