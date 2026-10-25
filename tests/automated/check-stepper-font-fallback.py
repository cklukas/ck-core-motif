#!/usr/bin/env python3
"""Check Stepper installs a drawable fallback text font."""
from pathlib import Path

source = Path("lib/Xm/Stepper.c").read_text(encoding="utf-8")
assert 'XLoadQueryFont(XtDisplay(widget), "fixed")' in source
assert "GCFont" in source
assert "XFreeFont(XtDisplay(widget), s->stepper.font)" in source
print("PASS Stepper installs and releases a fallback text font")
