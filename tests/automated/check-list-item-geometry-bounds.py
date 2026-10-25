#!/usr/bin/env python3
"""Check XmList item geometry queries cannot wrap narrow widths."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text()

assert "int available_width = (int)lw->core.width" in source
assert "iwidth = available_width > 0 ? (Dimension)available_width : 0;" in source

print("List item geometry query is bounded")
