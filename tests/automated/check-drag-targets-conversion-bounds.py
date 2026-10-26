#!/usr/bin/env python3
"""Ensure automatic TARGETS conversion handles size and allocation failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragC.c").read_text()
start = source.index("static Boolean \nDropConvertIncrCallback(")
body = source[start:]
assert body.count("INT_MAX / sizeof(Atom)") >= 2
assert body.count("exportTargets == NULL") >= 2
assert body.count("XtMalloc(len)") >= 2
assert body.count("*valueRtn == NULL") >= 2
print("PASS automatic drag TARGETS conversion checks bounds and allocation")
