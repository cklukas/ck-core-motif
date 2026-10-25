#!/usr/bin/env python3
"""Check that Screen child insertion validates parent/list state."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Screen.c"
text = source.read_text(encoding="utf-8")
for needle in (
    "if (wid == NULL)",
    "cw->desktop.num_children > cw->desktop.num_slots",
    "children == NULL && cw->desktop.num_slots != 0",
):
    if needle not in text:
        raise SystemExit(f"Screen child insertion guard is missing: {needle}")

print("Screen child insertion guards present")
