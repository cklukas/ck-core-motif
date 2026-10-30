#!/usr/bin/env python3
"""Check interactive control GC helpers refresh live background pixels."""
from pathlib import Path

root = Path(__file__).parents[2] / "lib" / "Xm"
regions = {
    "ProgressBar.c": "static void UpdateGCs(XmProgressBarWidget",
    "ProgressRing.c": "static void UpdateGCs(XmProgressRingWidget",
    "Spinner.c": "static void UpdateGCs(XmSpinnerWidget",
    "ToggleSwitch.c": "static void update_gcs(XmToggleSwitchWidget",
}
for name, marker in regions.items():
    source = (root / name).read_text()
    start = source.index(marker)
    end = source.index("}", start) + 1
    body = source[start:end]
    assert "XSetBackground" in body, name
    assert "core.background_pixel" in body, name

print("Interactive control GC helpers refresh background pixels")
