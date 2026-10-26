#!/usr/bin/env python3
"""Ensure clipboard target-list lengths are checked before int narrowing."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmeClipboardSource(")
end = source.index("/* XmeDragSource", start)
body = source[start:end]

assert body.count("value == NULL || size > (unsigned long)INT_MAX") == 2
assert body.count("count = (int)size;") == 2
assert body.count("XmClipboardEndCopy(display, XtWindow(w), itemid);") >= 4

print("PASS clipboard target-list lengths are bounded before narrowing")
