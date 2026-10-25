#!/usr/bin/env python3
"""Check MenuShell popup placement uses signed edge arithmetic."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "MenuShell.c").read_text()

assert "dispHeight - (int) rowcol->core.height" in source
assert "(int) old_x - (int) rowcol->core.width" in source
assert "(int) *x - (rightEdgeOfMenu - dispWidth + 1)" in source
assert "(int) *y - (bottomEdgeOfMenu - dispHeight + 1)" in source

print("MenuShell popup placement is signed")
