#!/usr/bin/env python3
"""Ensure DragBS property writers bound Xlib's signed length parameter."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragBS.c").read_text()
assert source.count("dataSize > UINT32_MAX || dataSize > (size_t)INT_MAX") == 2
assert source.count("(int)dataSize") >= 2
print("PASS DragBS property lengths are bounded")
