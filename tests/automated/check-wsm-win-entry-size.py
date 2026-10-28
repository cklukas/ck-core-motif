#!/usr/bin/env python3
"""Ensure WSM window-entry size calculation preserves both arrays."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)
start = text.index("WinEntrySizePacked(Display *dpy, int screen_num, WSMWinEntry *win_entry)")
end = text.index("\n}\n", start)
function = text[start:end]
assert "win_entry->num_windows < 0" in function
assert "AddPackedSize(&size," in function
assert "win_entry->num_match_properties" in function
assert "size  = sizeof(CARD32);\n    size += sizeof(WindowProperty)" not in function
print("WSM window-entry size validates counts and checked arithmetic")
