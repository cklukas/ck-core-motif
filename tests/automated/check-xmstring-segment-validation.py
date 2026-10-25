#!/usr/bin/env python3
"""Check segment extraction validates context before reading it."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringSeg.c").read_text()
start = source.index("_XmStringGetSegment(")
direction = source.index("_XmStrContDir(context)", start)
validation = source.index("if (! (context && text && tag && type", start)

assert validation < direction
assert "Validate context before compatibility output initialization" in source

print("PASS XmString segment extraction validates context before initialization")
