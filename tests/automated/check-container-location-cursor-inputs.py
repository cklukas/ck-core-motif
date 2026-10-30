#!/usr/bin/env python3
"""Check Container location-cursor restoration validates the parent."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.rindex("SetLocationCursor(\n")
region = source[start:source.index("\n}", start)]
assert region.index("if (cwid == NULL)") < region.index("parent = XtParent(cwid)")
assert region.index("if (parent == NULL)") < region.index("XmGetFocusWidget(parent)")
print("PASS Container location-cursor restoration validates parent input")
