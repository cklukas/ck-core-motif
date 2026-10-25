#!/usr/bin/env python3
"""Check icon move occupancy skips the invalid-place sentinel."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWinConf.c"
text = source.read_text(encoding="utf-8")

assert "place != NO_ICON_PLACE && pIPD->placeList != NULL" in text
assert "pIPD->placeList[place].pCD" in text
print("icon move occupancy skips invalid placement sentinels")
