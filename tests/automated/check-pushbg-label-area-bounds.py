#!/usr/bin/env python3
"""Check PushButtonGadget label rectangles stay non-negative."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "PushBG.c").read_text(encoding="utf-8")
start = source.index("\nComputePBLabelArea(\n")
end = source.index("\n}\n", start)
region = source[start:end]
assert "pb->rectangle.width > (Dimension)adjust" in region
assert "pb->rectangle.height > (Dimension)adjust" in region
assert "(int)(pb->rectangle.width - (Dimension)adjust)" in region
assert "(int)(pb->rectangle.height - (Dimension)adjust)" in region
assert "box->width = pb->rectangle.width - adjust" not in region
print("push-button gadget label geometry clamps tiny widgets")
