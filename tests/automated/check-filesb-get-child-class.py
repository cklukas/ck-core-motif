#!/usr/bin/env python3
"""Ensure XmFileSelectionBoxGetChild validates its widget class."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/FileSB.c").read_text()
start = source.index("XmFileSelectionBoxGetChild(")
start = source.index("XmFileSelectionBoxGetChild(\n        Widget fs,", start)
end = source.index("\n}\n", start) + 3
body = source[start:end]

assert "!XtIsSubclass(fs, xmFileSelectionBoxWidgetClass)" in body
assert body.index("!XtIsSubclass(fs, xmFileSelectionBoxWidgetClass)") < body.index("_XmWidgetToAppContext")
print("XmFileSelectionBoxGetChild validates its widget class")
