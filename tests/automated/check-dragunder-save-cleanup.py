#!/usr/bin/env python3
"""Check DragUnder backing-store failures leave cleanup state consistent."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DragUnder.c").read_text(
    encoding="utf-8"
)
if source.count("aSaveData->numSavedPixmaps = 0;") < 2:
    raise SystemExit("backing-store allocation failures do not reset saved count")

print("DragUnder save-cleanup checks passed")
