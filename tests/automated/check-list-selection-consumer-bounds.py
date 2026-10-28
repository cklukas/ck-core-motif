#!/usr/bin/env python3
"""Ensure List selection consumers share stale-position validation."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(
    encoding="utf-8"
)
assert "static Boolean selected_position_valid(XmListWidget lw, int index)" in source
assert "selectedPositions[index] > 0" in source
assert "selectedPositions[index] <= lw->list.itemCount" in source
assert source.count("if (!selected_position_valid(lw, i))") >= 6
print("PASS List selection consumers validate selected positions")
