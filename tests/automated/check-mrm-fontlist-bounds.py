#!/usr/bin/env python3
"""Check legacy Mrm font-list count-minus-one sizing."""

from pathlib import Path


root = Path(__file__).parents[2] / "lib" / "Mrm"
module = (root / "Mrmlread.c").read_text(encoding="utf-8")
writer = (root / "Mrmwcrw.c").read_text(encoding="utf-8")
needle = "(SIZE_MAX - sizeof(RGMFontList)) / sizeof(RGMFontItem)"
if module.count(needle) < 1 or writer.count(needle) < 2:
    raise SystemExit("legacy Mrm font-list capacity guards are incomplete")
if module.count("count <= 0") < 1 or writer.count("count <= 0") < 2:
    raise SystemExit("legacy Mrm font-list zero-count guards are incomplete")

print("Mrm legacy font-list sizing is bounded")
