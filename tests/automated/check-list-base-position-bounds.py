#!/usr/bin/env python3
"""Check List BaseX/BaseY margin arithmetic cannot wrap Position."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(
    encoding="utf-8"
)
assert "static Position bounded_position(long long value)" in source
assert source.count("BaseX = bounded_position") >= 3
assert source.count("BaseY = bounded_position") >= 3
assert "BaseX = (Position)" not in source
assert "BaseY = (Position)" not in source
print("PASS List BaseX/BaseY coordinates stay bounded")
