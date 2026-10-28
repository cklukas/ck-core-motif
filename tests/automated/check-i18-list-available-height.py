#!/usr/bin/env python3
"""Check I18List available-height layout arithmetic cannot underflow."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert "long long height;" in source
assert "if (height < 0)" in source
assert "height = (long long)ilist->core.height -" in source
print("PASS I18List available height stays signed")
