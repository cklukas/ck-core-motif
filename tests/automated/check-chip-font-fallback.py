#!/usr/bin/env python3
"""Check Chip installs a drawable fallback text font."""
from pathlib import Path

source = Path("lib/Xm/Chip.c").read_text(encoding="utf-8")
assert 'XLoadQueryFont(display, "fixed")' in source
assert "GCFont" in source
assert "XFreeFont(display, chip->chip.fallback_font)" in source
print("PASS Chip installs and releases a fallback text font")
