#!/usr/bin/env python3
"""Check drag client-window probing validates the X property request."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragC.c").read_text()
start = source.index("GetClientWindow(")
end = source.index("static void \nValidateDragOver", start)
body = source[start:end]

assert "result = XGetWindowProperty" in body
assert "result == Success && type != None" in body
assert "XFree(data);" in body

print("Drag client-window probing rejects failed property requests")
