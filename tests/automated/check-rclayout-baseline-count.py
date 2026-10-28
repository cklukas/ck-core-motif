#!/usr/bin/env python3
"""Check RowColumn baseline layout validates both baseline query results."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RCLayout.c").read_text()
assert source.count("line_count > 0 && baselines != NULL") == 2
assert "baselines[line_count - 1]" in source

print("RowColumn baseline layout guards empty baseline results")
