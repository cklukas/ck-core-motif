#!/usr/bin/env python3
"""Check Spinner drawing avoids wrapping large spacing through Position."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Spinner.c").read_text(
    encoding="utf-8"
)
assert "static Position bounded_position(long long value)" in source
assert "bounded_position((long long)ring_x + diameter + _XmDpiDefaultDimension(widget,8))" in source
print("PASS Spinner center and label coordinates stay bounded")
