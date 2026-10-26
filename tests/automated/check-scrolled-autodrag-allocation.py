#!/usr/bin/env python3
"""Check that scrolled-window auto-drag allocations are validated."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "ScrolledW.c"
text = source.read_text()

for needle in (
    "if (new_w->swindow.auto_drag_rects == NULL)",
    "new_w->swindow.auto_drag_model = XmAUTO_DRAG_DISABLED;",
    "if (dragProc == NULL || rects == NULL)",
    "if (rects == NULL)",
    "if (hrect == NULL || num_hrect == 0)",
    "if (vrect == NULL || num_vrect == 0)",
):
    if needle not in text:
        raise SystemExit(f"ScrolledW.c is missing {needle}")

print("scrolled-window auto-drag allocation and rectangle results are guarded")
