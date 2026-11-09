#!/usr/bin/env python3
"""Ensure late pointer actions cannot update a destroying ToggleSwitch."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ToggleSwitch.c").read_text()
functions = {
    "Release": ("static void Release(Widget w", "static void Enter", "activate="),
    "Enter": ("static void Enter(Widget w", "static void Leave", "hover="),
    "Leave": ("static void Leave(Widget w", "Boolean XmToggleSwitchGetState", "hover="),
}

for name, (start_marker, end_marker, mutation) in functions.items():
    start = source.index(start_marker)
    body = source[start:source.index(end_marker, start)]
    assert "w->core.being_destroyed" in body, name
    assert body.index("w->core.being_destroyed") < body.index(mutation), name

print("ToggleSwitch late pointer actions reject teardown state")
