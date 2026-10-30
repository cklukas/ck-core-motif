#!/usr/bin/env python3
"""Check FileSelectionBox conversion validates callback and widget inputs."""

from pathlib import Path


source = Path("lib/Xm/FileSB.c").read_text()
start = source.index("FSBConvert(Widget wid")
body = source[start:source.index("\n}", start)]

assert "if (wid == NULL || fsb == NULL || cs == NULL)" in body
assert "XtIsSubclass(fsb, xmFileSelectionBoxWidgetClass)" in body
assert "cs->status = XmCONVERT_REFUSE;" in body
assert "XInternAtoms(XtDisplay(wid)" in body

print("FileSelectionBox conversion validates callback inputs")
