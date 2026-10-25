#!/usr/bin/env python3
"""Check window configuration rejects invalid converted icon places."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWinConf.c"
text = source.read_text(encoding="utf-8")

assert "place < 0 || place >= pIPD->totalPlaces" in text
assert "pIPD->placeList == NULL" in text
assert "place = NO_ICON_PLACE;" in text
print("window configuration normalizes invalid icon-place results")
