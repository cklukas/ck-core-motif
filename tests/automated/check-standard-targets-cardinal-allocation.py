#!/usr/bin/env python3
"""Ensure standard target allocation uses Xt's Cardinal byte-size contract."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmeStandardTargets(")
end = source.index("\n/*\n * XmeStandardConvert", start)
region = source[start:end]

assert "target_count = (size_t)count + builtin_count;" in region
assert "(Cardinal)(sizeof(*targets) * target_count)" in region
assert "if (targets == NULL)" in region

print("PASS standard target allocation is Cardinal-bounded")
