#!/usr/bin/env python3
"""Check I18List visibility padding cannot wrap a short edge coordinate."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert "last = bounded_short((long long)last + HORIZONTAL_SPACE);" in source
assert "last += HORIZONTAL_SPACE;" not in source
print("PASS I18List visibility padding stays bounded")
