#!/usr/bin/env python3
"""Check Drag ICC property requests use Xlib's signed length type."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragICC.c").read_text()
start = source.index("_XmGetDragReceiverInfo(")
end = source.index("_XmReadDSFromStream(", start)
body = source[start:end]

assert "unsigned long \t\tbytesafter, lengthRtn;" in body
assert "long\t\t\tlength;" in body
assert "length = 100000L;" in body

print("Drag ICC property request length matches Xlib")
