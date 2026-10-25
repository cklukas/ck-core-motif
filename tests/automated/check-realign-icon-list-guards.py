#!/usr/bin/env python3
"""Check icon-list realignment validates dimensions and placement storage."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

assert "pIBD->IPD.placeList == NULL" in text
assert "newRows <= 0 || newCols <= 0" in text
assert "pIBD->IPD.iPlaceW <= 0 || pIBD->IPD.iPlaceH <= 0" in text
assert "pIBD->IPD.placementColX == NULL || pIBD->IPD.placementRowY == NULL" in text
print("icon-list realignment validates dimensions and placement storage")
