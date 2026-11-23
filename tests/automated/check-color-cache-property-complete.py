#!/usr/bin/env python3
"""Check color-cache properties reject truncated X data."""
from pathlib import Path
import re

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorObj.c").read_text()
start = source.index("\nColorCachePropertyExists(")
body = source[start:source.index("\n}", start)]

read = re.search(r"\b(\w+)\s*=\s*XGetWindowProperty\(", body)
assert read is not None
reject = body[read.end():body.index("result = FetchPixelData(")]
assert re.search(rf"\b{read[1]}\s*!=\s*Success\b", reject)
assert re.search(r"\bbytesafter\s*!=\s*0\b", reject)
assert re.search(r"if\s*\(value\s*!=\s*NULL\)\s*XFree\(value\);\s*return\s+False", reject)

print("Color-cache properties reject truncated data")
