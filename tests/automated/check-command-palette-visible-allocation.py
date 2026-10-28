#!/usr/bin/env python3
"""Ensure CommandPalette bounds its visible-index allocation."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "CommandPalette.c").read_text(
    encoding="utf-8"
)
start = source.index("static void rebuild")
body = source[start:source.index("\nstatic void natural", start)]
assert "SIZE_MAX / sizeof(Cardinal)" in body
assert "UINT_MAX / sizeof(Cardinal)" in body
assert "visible item index is too large" in body
print("PASS CommandPalette visible-index allocation bounds")
