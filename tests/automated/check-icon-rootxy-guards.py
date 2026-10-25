#!/usr/bin/env python3
"""Check icon root-coordinate conversion validates pointers and state."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

assert "if (pX == NULL || pY == NULL)" in text
assert "if (pCD == NULL || pCD->pSD == NULL)" in text
assert "P_ICON_BOX(pCD)->bBoardWidget != NULL" in text
print("icon root-coordinate conversion validates output and client state")
