#!/usr/bin/env python3
"""Ensure WSM property sizing uses the protocol's 32-bit width."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)
assert "num_window_properties * sizeof(CARD32)" in text
assert "num_attributes *\n\t\t    sizeof(CARD32)" in text
assert "sizeof(CARD32) * win_entry->num_match_properties" in text
assert "num_window_properties * sizeof(Window)" not in text
print("WSM wire sizing uses 32-bit property fields")
