#!/usr/bin/env python3
"""Check TreeView row geometry uses wide intermediates before X coordinates."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/TreeView.c").read_text(encoding="utf-8")
redisplay = source[source.rindex("static void Redisplay("):source.index("static int row_at(")]
assert "static int clamp_coordinate(long long value)" in source
assert "(long long)row *" in redisplay
assert "clamp_coordinate((long long)row" in redisplay
print("PASS TreeView row coordinates are widened and clamped")
