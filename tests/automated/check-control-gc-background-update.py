#!/usr/bin/env python3
"""Check interactive control GC helpers refresh live background pixels."""
from pathlib import Path

root = Path(__file__).parents[2] / "lib" / "Xm"
regions = {
    "ProgressBar.c": "static void UpdateGCs(XmProgressBarWidget",
    "ProgressRing.c": "static void UpdateGCs(XmProgressRingWidget",
    "Spinner.c": "static void UpdateGCs(XmSpinnerWidget",
}
for name, marker in regions.items():
    source = (root / name).read_text()
    start = source.index(marker)
    next_static = source.find("\nstatic ", start + len(marker))
    body = source[start:] if next_static < 0 else source[start:next_static]
    assert "XSetBackground" in body, name
    assert "core.background_pixel" in body, name

print("Interactive control GC helpers refresh background pixels")
