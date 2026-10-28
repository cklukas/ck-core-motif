#!/usr/bin/env python3
"""Check XPM header tables use the pointer count for calloc."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrDatFrI.c").read_text()
assert "XpmCalloc(header_nlines, sizeof(char *))" in source
assert "XpmCalloc(header_size, sizeof(char *))" not in source
print("PASS XPM header-table calloc count")
