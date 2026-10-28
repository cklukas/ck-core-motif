#!/usr/bin/env python3
"""Check XPM whitespace classification is safe for EOF and high-bit bytes."""
from pathlib import Path

data = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmdata.c").read_text(
    encoding="utf-8"
)
parser = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmparse.c").read_text(
    encoding="utf-8"
)
assert "XpmIsSpace(int c)" in data
assert "c != EOF && isspace((unsigned char)c)" in data
assert "isspace(c)" not in data
assert "isspace((unsigned char)*s2)" in parser
print("PASS XPM whitespace classification handles byte values safely")
