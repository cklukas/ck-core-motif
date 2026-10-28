#!/usr/bin/env python3
"""Check I18List left-of-window movement cannot narrow-wrap."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert "bounded_short(-((long long)before - start))" in source
assert "(0 - (before - start)), False, 0" not in source
print("PASS I18List left visibility movement stays bounded")
