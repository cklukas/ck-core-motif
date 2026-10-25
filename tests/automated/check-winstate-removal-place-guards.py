#!/usr/bin/env python3
"""Check state cleanup bounds-checks stored icon placements."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWinState.c"
text = source.read_text(encoding="utf-8")

assert "pWS_i->IPData.placeList != NULL" in text
assert "iplace >= 0 && iplace < pWS_i->IPData.totalPlaces" in text
assert "pWS->IPData.placeList != NULL" in text
assert "ICON_PLACE(pCD) >= 0" in text
print("state cleanup validates stored icon placements")
