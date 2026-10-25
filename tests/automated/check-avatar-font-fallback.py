#!/usr/bin/env python3
"""Check Avatar initials rendering has a drawable fallback font."""
from pathlib import Path

source = Path("lib/Xm/Avatar.c").read_text(encoding="utf-8")
assert 'XLoadQueryFont(XtDisplay(widget), "fixed")' in source
assert "GCFont" in source
assert "XFreeFont(XtDisplay(widget), avatar->avatar.fallback_font)" in source
print("PASS Avatar installs and releases a fallback text font")
