#!/usr/bin/env python3
"""Ensure EmptyState layout positions use wide, bounded arithmetic."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "EmptyState.c").read_text(
    encoding="utf-8"
)
assert "static Position bounded_position(unsigned long value)" in source
start = source.index("static void layout(XmEmptyStateWidget state)")
body = source[start : source.index("\nstatic void negotiate", start)]
assert "unsigned long y" in body
assert body.count("bounded_position(y)") == 3
print("PASS EmptyState layout positions are bounded safely")
