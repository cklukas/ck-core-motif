#!/usr/bin/env python3
"""Check icon placement conversions reject invalid layout state."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIPlace.c"
text = source.read_text(encoding="utf-8")

assert "pIPD == NULL" in text
assert "placementCols <= 0" in text
assert "iPlaceW <= 0" in text
assert "return (NO_ICON_PLACE);" in text
assert "placementColX == NULL" in text
print("icon placement conversions validate layout state")
