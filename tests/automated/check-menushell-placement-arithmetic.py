#!/usr/bin/env python3
"""Check MenuShell edge calculations do not wrap unsigned dimensions."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "MenuShell.c").read_text()

assert "int rightEdgeOfMenu, bottomEdgeOfMenu;" in source
assert "(int)*x + (int)RowColBorderWidth" in source
assert "(int)*y + (int)RowColBorderWidth" in source

print("MenuShell placement arithmetic is bounded")
