#!/usr/bin/env python3
"""Check NumberBox arrow coordinates stay representable as X11 shorts."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "NumberBox.c").read_text()
start = source.index("static void Redisplay")
body = source[start:source.index("static void commit_text", start)]
assert "int arrow_x" in body
assert "arrow_x > SHRT_MAX - 19" in body
assert "arrow_x = SHRT_MAX - 19" in body
print("PASS NumberBox arrow coordinates are bounded")
