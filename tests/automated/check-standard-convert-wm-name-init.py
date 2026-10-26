#!/usr/bin/env python3
"""Ensure standard WM_NAME conversion initializes failed property reads."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("} else if (atoms[XmANAME] == cs -> target) {")
body = source[start:source.index("} else if (atoms[XmACLIENT_WINDOW]", start)]
assert "Atom type = None;" in body
assert "int format = 0;" in body
assert "total_value = value != NULL" in body
print("PASS standard WM_NAME conversion handles missing properties deterministically")
