#!/usr/bin/env python3
"""Check InitIconPlacement validates state before dimension division."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIPlace.c"
text = source.read_text(encoding="utf-8")

start = text.index("void InitIconPlacement")
guard = text[start:text.index("xMargin =", start)]
assert "pWS == NULL" in guard
assert "pWS->pSD == NULL" in guard
assert "iconWidth <= 0 || pWS->pSD->iconHeight <= 0" in guard
assert "wmGD.iconAutoPlace = False" in guard
print("InitIconPlacement validates workspace and icon dimensions")
