#!/usr/bin/env python3
"""Check GrabShell geometry accumulation starts from defined dimensions."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "GrabShell.c").read_text()
start = source.index("ChangeManaged(Widget wid)")
end = source.index("\n}\n", start) + 2
function = source[start:end]

assert "XtWidgetGeometry  pref, mygeom = { 0 }, replygeom;" in function
assert "mygeom.width +=" in function
assert "mygeom.height +=" in function
print("PASS GrabShell geometry initialization")
