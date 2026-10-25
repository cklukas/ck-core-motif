#!/usr/bin/env python3
"""Check XmPushButton internal unhighlight bounds before subtraction."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "PushB.c").read_text()
start = source.index("static void \nBorderUnhighlight(")
end = source.index("static int \nAdjustHighLightThickness(", start)
section = source[start:end]

assert "pb->core.width <= 2 * delta" in section
assert "pb->core.height <= 2 * delta" in section
assert section.index("pb->core.width <= 2 * delta") < section.index("width = pb->core.width - 2 * delta")
print("PASS PushButton unhighlight bounds")
