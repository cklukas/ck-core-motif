#!/usr/bin/env python3
"""Check that Mwm command-tree names use checked bounded construction."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmMenu.c").read_text(
    encoding="utf-8"
)
start = source.index("static void InsertTreeOnClient")
region = source[start:source.index("/* If there is an exclusion", start)]
assert "command_length > SIZE_MAX - name_length" in region
assert "XtMalloc((Cardinal) length)" in region
assert 'snprintf(new_command_str, length, "%s.<%s>"' in region
assert "sprintf(new_command_str" not in region
print("Mwm command-tree construction is bounded")
