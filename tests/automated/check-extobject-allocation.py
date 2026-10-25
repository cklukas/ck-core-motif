#!/usr/bin/env python3
"""Guard ExtObject initialization against extension-record allocation failure."""

from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "ExtObject.c").read_text(
    encoding="utf-8"
)
start = text.index("static void \nInitialize(")
body = text[start:text.index("\n}\n", start) + 2]
allocation = "extData = (XmWidgetExtData) XtCalloc(1, sizeof(XmWidgetExtDataRec));"
if allocation not in body:
    raise SystemExit("ExtObject extension allocation not found")
if "if (extData == NULL)" not in body or "return;" not in body:
    raise SystemExit("ExtObject allocation failure is not handled")
if body.index("if (extData == NULL)") > body.index("_XmPushWidgetExtData"):
    raise SystemExit("ExtObject registers extension data before checking it")

print("ExtObject allocation checks passed")
