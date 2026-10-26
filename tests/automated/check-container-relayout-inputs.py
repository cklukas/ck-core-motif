#!/usr/bin/env python3
"""Check XmContainerRelayout validates its widget."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("XmContainerRelayout(")
region = source[start:]
assert "if (wid == NULL)" in region
assert region.index("if (wid == NULL)") < region.index("_XmWidgetToAppContext(wid)")
print("PASS XmContainerRelayout validates null widgets")
