#!/usr/bin/env python3
"""Check CommandPalette preserves compound labels in visible list entries."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CommandPalette.c").read_text(
    encoding="utf-8"
)
if "static XmString make_display_string" not in source:
    raise SystemExit("CommandPalette compound display helper is missing")
if "XmStringConcat(result, suffix)" not in source:
    raise SystemExit("CommandPalette must concatenate XmString segments")
if "display = make_display_string(palette->palette.items[i].label" not in source:
    raise SystemExit("CommandPalette list insertion must use the compound display helper")
if "display = XmStringCreateLocalized(combined);" in source:
    raise SystemExit("CommandPalette must not rebuild visible labels from flattened bytes")

print("CommandPalette preserves compound labels and shortcuts")
