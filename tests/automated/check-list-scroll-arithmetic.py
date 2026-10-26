#!/usr/bin/env python3
"""Check List scrolling avoids signed overflow in value/increment arithmetic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text()
up_start = source.index("void\nListScrollUp")
down_start = source.index("void\nListScrollDown", up_start)
up = source[up_start:down_start]
down = source[down_start:source.index("\f", down_start)]
assert "int64_t candidate;" in up
assert "candidate = (int64_t)value - (int64_t)increment;" in up
assert "candidate > INT_MAX" in up
assert "int64_t candidate;" in down
assert "candidate = (int64_t)value + (int64_t)increment;" in down
assert "int64_t maximum;" in down
assert "candidate < INT_MIN" in down
print("PASS List scrolling bounds value/increment arithmetic")
