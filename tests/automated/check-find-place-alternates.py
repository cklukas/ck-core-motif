#!/usr/bin/env python3
"""Check alternate icon-place probes validate converted indices."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIPlace.c"
text = source.read_text(encoding="utf-8")

assert "place = CvtIconPositionToPlace (pIPD, altX, altY);" in text
assert "place >= 0 && place < pIPD->totalPlaces" in text
print("alternate icon-place probes validate converted indices")
