#!/usr/bin/env python3
"""Ensure TabBox's BASIC converter result cannot inherit prior state."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TabBox.c").read_text(
    encoding="utf-8"
)
start = source.index("CvtStringToTabMode(Display *dpy")
end = source.index("CvtStringToTabEdge(Display *dpy", start)
body = source[start:end]

basic = body.index('XmCompareISOLatin1(str, "BASIC")')
basic_branch = body.index("{", basic)
basic_assignment = body.index("result = XmTABS_BASIC;", basic_branch)
stacked = body.index('XmCompareISOLatin1(str, "STACKED")')

assert basic < basic_branch < basic_assignment < stacked

print("PASS TabBox BASIC conversion resets the static mode result")
