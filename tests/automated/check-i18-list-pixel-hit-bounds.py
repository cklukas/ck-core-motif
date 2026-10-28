#!/usr/bin/env python3
"""Check I18List pixel-to-column hit testing uses a wide accumulator."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert "long long cur_col;" in source
assert "if (cur_col < (long long)x)" in source
assert "if (cur_col > (long long)x)" in source
assert "short cur_col;" not in source
print("PASS I18List pixel hit testing stays wide")
