#!/usr/bin/env python3
"""Ensure WSM property sizing uses the protocol's 32-bit width."""

from pathlib import Path
import re


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)
flat = re.sub(r"\s+", " ", text)
assert re.search(r"num_window_properties.*sizeof\(CARD32\)", flat)
assert re.search(r"num_attributes.*sizeof\(CARD32\)", flat)
assert re.search(r"sizeof\(CARD32\).*win_entry->num_match_properties", flat)
assert "num_window_properties * sizeof(Window)" not in text
print("WSM wire sizing uses 32-bit property fields")
