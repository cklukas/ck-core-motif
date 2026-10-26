#!/usr/bin/env python3
"""Check XmUpdateDisplay rejects a null widget before display lookup."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TrackLoc.c").read_text(encoding="utf-8")
start = source.index("XmUpdateDisplay(")
region = source[start:]
assert "if (w == NULL)" in region
assert region.index("if (w == NULL)") < region.index("XtDisplay(w)")
print("PASS XmUpdateDisplay validates null widgets")
