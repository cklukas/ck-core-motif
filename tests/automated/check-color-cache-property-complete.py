#!/usr/bin/env python3
"""Check color-cache properties reject truncated X data."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorObj.c").read_text()
start = source.rindex("ColorCachePropertyExists(")
body = source[start:]

assert "result != Success" in body
assert "bytesafter != 0" in body
assert "XFree(value);" in body

print("Color-cache properties reject truncated data")
