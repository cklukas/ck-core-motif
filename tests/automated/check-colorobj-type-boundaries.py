#!/usr/bin/env python3
"""Static regression checks for XmColorObj allocation and result types."""

from pathlib import Path
import re
import sys


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "ColorObj.c").read_text()


def require(pattern: str, description: str) -> None:
    if not re.search(pattern, source, re.S):
        print(f"missing {description}", file=sys.stderr)
        raise SystemExit(1)


require(
    r"if \(nscreens > 0\) \{.*?XtCalloc\(\(Cardinal\)nscreens, sizeof\(Colors\)\).*?"
    r"XtCalloc\(\(Cardinal\)nscreens, sizeof\(Atom\)\).*?"
    r"XtCalloc\(\(Cardinal\)nscreens, sizeof\(int\)\)",
    "positive screen-count allocation guard",
)
require(
    r"savetimeout != \(unsigned long\)-1",
    "unsigned selection-timeout sentinel comparison",
)
require(
    r"int x_result\s*;\s*Boolean result = False\s*;",
    "separate Xlib status and Boolean declarations",
)
require(
    r"x_result = XGetWindowProperty\(.*?if \(\(x_result != Success\).*?"
    r"result = FetchPixelData\(.*?return result;",
    "separate Xlib status and Boolean result",
)
