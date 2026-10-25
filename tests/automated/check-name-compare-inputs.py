#!/usr/bin/env python3
"""Check that XmeNamesAreEqual validates both string inputs."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c").read_text(
    encoding="utf-8"
)
start = text.index("Boolean \nXmeNamesAreEqual(")
body = text[start : start + 650]
if "in_str == NULL || test_str == NULL" not in body:
    raise SystemExit("name comparison input guard is missing")

print("name comparison input guard present")
