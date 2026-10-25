#!/usr/bin/env python3
"""Ensure WSM window-entry unpacking survives allocation failure."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)
start = text.index("UnpackWinEntry(MessageData *data")
end = text.index("\n}\n", start)
function = text[start:end]
assert "win_entry->windows = NULL;" in function
assert "(void) UnpackWindow(data);" in function
assert "win_entry->match_properties = NULL;" in function
assert "(void) UnpackProperty(data);" in function
print("WSM window-entry unpacking handles allocation failure")
