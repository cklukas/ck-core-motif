#!/usr/bin/env python3
"""Check FindIconPlace rejects the invalid-place sentinel before indexing."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIPlace.c"
text = source.read_text(encoding="utf-8")

assert "place = CvtIconPositionToPlace (pIPD, x, y);" in text
assert "if (place < 0)" in text
assert "return (NO_ICON_PLACE);" in text
print("icon-place search rejects invalid coordinate sentinels before indexing")
