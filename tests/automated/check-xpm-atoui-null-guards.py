#!/usr/bin/env python3
"""Check XPM decimal parsing rejects null inputs without dereferencing them."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmmisc.c").read_text()
start = source.index("xpmatoui(")
region = source[start : source.index("n = 0;", start) + len("n = 0;")]

assert "if (p == NULL || ui_return == NULL)" in region
assert region.index("if (p == NULL || ui_return == NULL)") < region.index("n = 0;")
print("XPM decimal parsing guards null inputs")
