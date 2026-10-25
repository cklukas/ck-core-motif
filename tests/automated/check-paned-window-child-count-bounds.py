#!/usr/bin/env python3
"""Check maintained PanedWindow child traversals use Cardinal indices."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "PanedW.c").read_text(
    encoding="utf-8"
)
for signature in (
    "ConstraintDestroy(",
    "AdjustGC(",
    "ReManageChildren(\n        XmPanedWindowWidget pw",
):
    start = source.index(signature)
    start = source.index("{", start)
    end = source.find("\n}\n", start)
    if end == -1:
        end = len(source)
    region = source[start:end]
    if "int i" in region or "register int i" in region:
        raise SystemExit(f"signed child index remains in {signature}")
assert source.count("Cardinal i;") >= 3
print("maintained PanedWindow child traversals use Cardinal indices")
