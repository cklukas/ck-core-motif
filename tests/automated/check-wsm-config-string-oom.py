#!/usr/bin/env python3
"""Ensure WSM config parsing rolls back after string allocation failure."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)
needle = "String str = UnpackString(&data);"
start = text.index(needle)
end = text.index("fmt->attr_list[i].nameq", start)
block = text[start:end]
assert "if (str == NULL)" in block
assert "_WSMClearConfigScreenInfo(dpy, screen_num);" in block
print("WSM config parsing rolls back on string allocation failure")
