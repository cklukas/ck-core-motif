#!/usr/bin/env python3
"""Check the toggle-switch knob is vertically centered from its dimensions."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/ToggleSwitch.c").read_text(
    encoding="utf-8"
)
if "int knob_y = track_y +\n        ((int)track_height - (int)TOGGLE_SWITCH_KNOB_DIAMETER) / 2;" not in source:
    raise SystemExit("toggle-switch knob must use a centered dimension-derived y offset")
if "int knob_y = track_y + 4;" in source:
    raise SystemExit("toggle-switch still contains the hard-coded y offset")

print("ToggleSwitch centering check passed")
