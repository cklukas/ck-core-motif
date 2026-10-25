#!/usr/bin/env python3
"""Check that Mwm menu-info labels fit their fixed protocol buffers."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmCmd.c").read_text(
    encoding="utf-8"
)
assert source.count("snprintf(itemName[") >= 3
assert "strcpy(itemName[n]" not in source
assert "strcpy (itemName[i]" not in source
print("Mwm menu-info labels are copied with bounded formatting")
