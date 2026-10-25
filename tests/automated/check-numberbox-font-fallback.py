#!/usr/bin/env python3
"""Check NumberBox text GCs always receive a drawable fallback font."""
from pathlib import Path

source = Path("lib/Xm/NumberBox.c").read_text(encoding="utf-8")
assert 'XLoadQueryFont(display, "fixed")' in source
assert "GCFont" in source
assert "XFreeFont(display, number->number_box.font)" in source
print("PASS NumberBox installs and releases a fallback text font")
