#!/usr/bin/env python3
"""Check XPM header copying stays within the allocated line table."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrDatFrI.c").read_text()
start = source.index("/* can header have less elements")
end = source.index("/* print pixels */", start)
body = source[start:end]
assert "l < header_nlines" in body
assert "if (l + 1 < header_nlines)" in body
assert "l <= n" not in body
print("PASS XPM header successor bounds")
