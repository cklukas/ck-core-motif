#!/usr/bin/env python3
"""Check Link activation rejects null widget handles before dereferencing."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Link.c").read_text(encoding="utf-8")
start = source.index("void XmLinkActivate")
region = source[start:]
assert "if (widget == NULL || !XmIsLink(widget)" in region
assert region.index("widget == NULL") < region.index("widget->core.being_destroyed")
print("PASS Link activation validates null widgets before dereferencing")
