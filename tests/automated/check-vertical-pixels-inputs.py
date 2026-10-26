#!/usr/bin/env python3
"""Check vertical resource conversion validates widget and output inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ResInd.c").read_text(encoding="utf-8")
start = source.index("XmeToVerticalPixels(")
region = source[start:]
assert "if (widget == NULL || value == NULL)" in region
assert region.index("if (widget == NULL || value == NULL)") < region.index("_XmWidgetToAppContext(widget)")
print("PASS XmeToVerticalPixels validates input pointers")
