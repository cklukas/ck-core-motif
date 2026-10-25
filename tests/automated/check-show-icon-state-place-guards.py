#!/usr/bin/env python3
"""Check minimized-icon state validates workspace placement storage."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWinState.c"
text = source.read_text(encoding="utf-8")

assert "if (pWS == NULL || pCD == NULL)" in text
assert "pWS->IPData.placeList == NULL" in text
assert "ICON_PLACE(pCD) >= pWS->IPData.totalPlaces" in text
print("minimized-icon state validates workspace placement storage")
