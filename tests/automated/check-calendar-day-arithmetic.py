#!/usr/bin/env python3
"""Ensure Calendar day navigation uses wide, clamped ordinal arithmetic."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Calendar.c").read_text(
    encoding="utf-8"
)
start = source.index("static XmCalendarDate add_days")
body = source[start : source.index("\nstatic void normalize_visible", start)]
assert "long long ordinal" in body
assert "(long long)delta" in body
assert "const int minimum" in body
assert "const int maximum" in body
assert "if (ordinal < minimum)" in body
assert "if (ordinal > maximum)" in body
print("PASS Calendar day navigation clamps wide ordinal arithmetic")
