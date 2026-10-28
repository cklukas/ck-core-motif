#!/usr/bin/env python3
"""Check I18List total-width accumulation saturates its legacy return type."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert "long long width;" in source
assert "if (width > SHRT_MAX)" in source
assert "return SHRT_MAX;" in source
assert "return((short) width);" not in source
print("PASS I18List total width cannot wrap short")
