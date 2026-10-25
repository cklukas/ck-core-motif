#!/usr/bin/env python3
"""Check SetNewBounds validates icon-box state before division."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

start = text.index("void SetNewBounds")
guard = text[start:text.index("cw =", start)]
assert "pIBD == NULL" in guard
assert "bBoardWidget == NULL" in guard
assert "widthInc <= 0" in guard
assert "lastCol = 0" in guard
print("SetNewBounds validates icon-box state and increments")
