#!/usr/bin/env python3
"""Ensure scaled destination images use the target drawable's screen."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("void _XmPutScaledImage")
end = source.index("\n\n\n/*** COLOR CACHE NOW ***/", start)
region = source[start:end]

assert "XGetGeometry(display, d, &root" in region
assert "RootWindow(display, (int)candidate) == root" in region
assert "ScreenOfDisplay(display, screen_number)" in region
assert "DefaultScreenOfDisplay(display)" not in region

print("PASS image scaling selects the target drawable screen")
