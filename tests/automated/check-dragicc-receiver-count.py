#!/usr/bin/env python3
"""Check Drag ICC receiver counts are validated before wire narrowing."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragICC.c").read_text()
start = source.index("_XmSetDragReceiverInfo(")
body = source[start:]

assert "int\t\t\ttreeDropSites;" in body
assert "if (treeDropSites < 0)" in body
assert "numDropSites = (Cardinal)treeDropSites;" in body
assert "num_drop_sites = (CARD16)numDropSites;" in body
assert "numDropSites > UINT16_MAX" in body

print("Drag ICC receiver counts are wire-bounded")
