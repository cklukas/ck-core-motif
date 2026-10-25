#!/usr/bin/env python3
"""Check ComboBox popup and arrow geometry for collapsed controls."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ComboBox.c").read_text()

assert "shell_width = (XtWidth(cb) > 2 * CB_HighlightThickness(cb)) ?" in source
assert "CB_HitRect(cb).x = (Position) ((int) XtWidth(cb) -" in source

print("ComboBox collapsed geometry is bounded")
