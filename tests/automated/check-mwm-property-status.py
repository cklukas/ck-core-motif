#!/usr/bin/env python3
"""Check Motif WM detection validates the X property request status."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IsMwmRun.c").read_text()
start = source.index("XmIsMotifWMRunning(")
body = source[start:]

assert "result = XGetWindowProperty" in body
assert "result != Success" in body
assert "if ((result != Success)" in body
assert "XFree((char *)prop);" in body

print("Motif WM detection rejects failed property requests")
