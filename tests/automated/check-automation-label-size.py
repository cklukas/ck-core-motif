#!/usr/bin/env python3
"""Check complete label sizing in Mwm automation replies."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmCmd.c").read_text(
    encoding="utf-8"
)
assert source.count("strlen(menu_item->label)") >= 3
assert "strlen(menu_item->label + 1)" not in source
print("Mwm automation replies size complete menu labels")
