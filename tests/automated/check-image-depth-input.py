#!/usr/bin/env python3
"""Check scaled-pixmap construction rejects invalid zero depth."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("_XmGetScaledPixmap(")
end = source.index("    if (!isfinite(scaling_ratio)", start)
body = source[start:end]

assert "depth == 0 || depth == INT_MIN" in body
assert "return (XmUNSPECIFIED_PIXMAP);" in body
print("scaled-pixmap depth input is validated")
