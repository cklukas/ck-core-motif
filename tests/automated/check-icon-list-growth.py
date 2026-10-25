#!/usr/bin/env python3
"""Check icon-list growth rejects invalid and unrepresentable sizes."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

assert "incr <= 0" in text
assert "incr > INT_MAX - pIBD->IPD.totalPlaces" in text
assert "newSize > UINT_MAX / sizeof(IconInfo)" in text
print("icon-list growth validates signed and allocation bounds")
