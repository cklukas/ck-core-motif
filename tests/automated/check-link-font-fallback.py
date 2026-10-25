#!/usr/bin/env python3
"""Check Link installs a drawable fallback text font."""
from pathlib import Path

source = Path("lib/Xm/Link.c").read_text(encoding="utf-8")
assert 'XLoadQueryFont(XtDisplay(widget), "fixed")' in source
assert "GCFont" in source
assert "XFreeFont(XtDisplay(widget), link->link.font)" in source
print("PASS Link installs and releases a fallback text font")
