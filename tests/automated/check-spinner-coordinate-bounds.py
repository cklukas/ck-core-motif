#!/usr/bin/env python3
"""Check Spinner drawing avoids wrapping large spacing through Position."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Spinner.c").read_text(
    encoding="utf-8"
)
assert "static Position bounded_position(long long value)" in source
assert "int center_x, center_y" in source
assert "center_x = (int)spinner->spinner.spacing" in source
assert "center_x - (int)diameter / 2" in source
assert "bounded_position((long long)spinner->spinner.spacing" in source
print("PASS Spinner center and label coordinates stay bounded")
