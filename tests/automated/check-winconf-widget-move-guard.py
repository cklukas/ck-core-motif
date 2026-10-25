#!/usr/bin/env python3
"""Check window configuration validates the widget placement before moving."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWinConf.c"
text = source.read_text(encoding="utf-8")

assert "ICON_PLACE(pcd) >= 0" in text
assert "ICON_PLACE(pcd) < pIPD->totalPlaces" in text
assert "pIPD->placeList[ICON_PLACE(pcd)].theWidget != NULL" in text
print("window configuration validates the widget placement before moving")
