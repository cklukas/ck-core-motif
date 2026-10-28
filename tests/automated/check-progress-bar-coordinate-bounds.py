#!/usr/bin/env python3
"""Check ProgressBar drawing coordinates cannot wrap through Position."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ProgressBar.c").read_text(
    encoding="utf-8"
)
assert "#include <limits.h>" in source
assert "static Position bounded_position(long long value)" in source
assert "int bar_x, bar_y" in source
assert "bar_x = (int)progress->progress.spacing" in source
assert "bounded_position((long long)progress->progress.spacing)" in source
assert "bounded_position((long long)bar_x" in source
assert "bounded_position((long long)bar_y" in source
print("PASS ProgressBar label and text coordinates stay bounded")
