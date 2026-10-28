#!/usr/bin/env python3
"""Ensure Toolbar natural width accumulation saturates before narrowing."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Toolbar.c").read_text(
    encoding="utf-8"
)
start = source.index("static void natural(XmToolbarWidget toolbar")
body = source[start : source.index("\nstatic Cardinal choose_actions", start)]
assert "addition > USHRT_MAX - total" in body
assert "? USHRT_MAX : total + addition" in body
print("PASS Toolbar natural width accumulation saturates safely")
