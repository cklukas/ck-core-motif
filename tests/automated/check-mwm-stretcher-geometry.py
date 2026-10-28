#!/usr/bin/env python3
"""Check that MWM stretcher creation validates frame-part geometry."""

from pathlib import Path


source = Path("clients/mwm/WmCDecor.c").read_text(encoding="utf-8")
start = source.index("Boolean CreateStretcherWindows (ClientData *pcd)")
end = source.index("void CountFrameRectangles", start)
body = source[start:end]

assert "Boolean framePartValid;" in body
assert body.count("framePartValid = GetFramePartInfo") == 8
assert "if (!framePartValid)" in body
assert body.index("if (!framePartValid)") < body.index("XCreateWindow")

print("PASS MWM stretcher geometry is validated before window creation")
