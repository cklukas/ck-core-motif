#!/usr/bin/env python3
"""Check XmPushButton validates its selected default shadow delta."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "PushB.c").read_text()
start = source.index("static void \nDrawDefaultButtonShadows(")
end = source.index("static void \nBorderHighlight(", start)
shadow = source[start:end]

assert "pb->core.width <= 2 * delta" in shadow
assert "pb->core.height <= 2 * delta" in shadow
assert shadow.index("pb->core.width <= 2 * delta") < shadow.index("width = pb->core.width - 2 * delta")
print("PASS PushButton default shadow bounds")
