#!/usr/bin/env python3
"""Check XmPushButton menu unhighlight drawing prerequisites."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "PushB.c").read_text()
start = source.index("static void \nBorderUnhighlight(")
end = source.index("static int \nAdjustHighLightThickness(", start)
section = source[start:end]

assert "pb->pushbutton.background_gc != NULL" in section
assert "pb->core.width != 0 && pb->core.height != 0" in section
assert "pb->core.width > 2 * pb->primitive.highlight_thickness" in section
assert "pb->core.height > 2 * pb->primitive.highlight_thickness" in section
print("PASS PushButton menu unhighlight guards")
