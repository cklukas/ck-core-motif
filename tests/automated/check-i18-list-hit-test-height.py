#!/usr/bin/env python3
"""Check I18List motion hit testing does not narrow widget height to short."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert "if ((long long)y > (long long)w->core.height)" in source
assert "if (y > ((short) w->core.height))" not in source
print("PASS I18List hit-test height comparison stays wide")
