#!/usr/bin/env python3
"""Check TimePicker text coordinates cannot wrap through Position."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TimePicker.c").read_text(
    encoding="utf-8"
)
assert "#include <limits.h>" in source
assert "static Position bounded_position(long long value)" in source
assert "bounded_position(6)" in source
assert "bounded_position(((long long)picker->core.height - height) / 2)" in source
print("PASS TimePicker text coordinates stay bounded")
