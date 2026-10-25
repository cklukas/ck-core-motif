#!/usr/bin/env python3
"""Check PushButton label rectangles cannot wrap on tiny widgets."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "PushB.c").read_text(encoding="utf-8")
start = source.index("\nComputePBLabelArea(\n")
start = source.index("{", start)
end = source.index("\n}\n", start)
region = source[start:end]
assert "pb->core.width > (Dimension)adjust" in region
assert "pb->core.height > (Dimension)adjust" in region
assert "?\n      (pb->core.width - (Dimension)adjust) : 0" in region
assert "?\n      (pb->core.height - (Dimension)adjust) : 0" in region
assert "box->width  = pb->core.width - adjust" not in region
print("push-button label geometry clamps tiny widgets")
