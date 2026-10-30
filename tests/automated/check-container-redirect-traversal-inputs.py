#!/usr/bin/env python3
"""Check Container traversal redirection validates its clip parent."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.rindex("RedirectTraversal(\n")
region = source[start:source.index("\n}", start)]
assert region.index("clip = XtParent(cw)") < region.index("XmIsClipWindow")
assert region.index("if (clip == NULL)") < region.index("XmIsClipWindow")
print("PASS Container traversal redirection validates clip parent")
