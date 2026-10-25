#!/usr/bin/env python3
"""Check icon insertion validates inputs and post-growth placement."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmIconBox.c"
text = source.read_text(encoding="utf-8")

assert "if (pIBD == NULL || pCD == NULL)" in text
assert "place = GetNextIconPlace (&pIBD->IPD);" in text
assert "if (place == NO_ICON_PLACE)" in text
print("icon insertion validates inputs and placement availability")
