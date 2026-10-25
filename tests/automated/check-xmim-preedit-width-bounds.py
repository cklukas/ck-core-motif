#!/usr/bin/env python3
"""Check input-method preedit width avoids unsigned wraparound."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text()

assert "int available_width;" in source
assert "available_width = (int) vw->core.width - icp->status_width;" in source
assert "if (available_width < 0)" in source
assert "icp->preedit_width = MIN((int) rp->width, available_width);" in source

print("Xm input-method preedit width is bounded")
