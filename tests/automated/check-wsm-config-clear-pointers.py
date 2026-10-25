#!/usr/bin/env python3
"""Ensure WSM config rollback clears freed attribute pointers."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "disp.c").read_text(
    encoding="utf-8"
)
start = text.index("_WSMClearConfigScreenInfo")
end = text.index("\n}\n", start)
function = text[start:end]
for name in ("global", "window", "icon"):
    assert f"screen_info->{name}.attr_list = NULL;" in function
print("WSM config rollback clears attribute pointers")
