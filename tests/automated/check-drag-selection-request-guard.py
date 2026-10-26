#!/usr/bin/env python3
"""Ensure drag selection callbacks handle missing request events."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragC.c").read_text()
start = source.index("static Boolean \nDropConvertIncrCallback(")
body = source[start:]
assert body.count("XSelectionRequestEvent *request;") >= 2
assert body.count("request = XtGetSelectionRequest") >= 2
assert body.count("if (request == NULL)") >= 2
print("PASS drag selection callbacks guard missing request events")
