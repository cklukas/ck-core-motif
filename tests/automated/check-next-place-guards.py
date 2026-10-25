#!/usr/bin/env python3
"""Check icon-place lookup rejects missing lists and invalid indices."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIPlace.c"
text = source.read_text(encoding="utf-8")

assert "pIPD == NULL || pIPD->placeList == NULL" in text
assert "totalPlaces <= 0" in text
assert "place >= pIPD->totalPlaces" in text
assert text.count("return (NO_ICON_PLACE);") >= 3
print("icon-place lookup validates placement storage and indices")
