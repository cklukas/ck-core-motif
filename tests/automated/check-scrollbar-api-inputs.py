#!/usr/bin/env python3
"""Check ScrollBar value APIs reject non-ScrollBar widgets."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScrollBar.c").read_text(
    encoding="utf-8"
)

for name in ("XmScrollBarGetValues", "XmScrollBarSetValues"):
    start = source.index(name + "(")
    body = source[start : source.index("\n}\n", start) + 3]
    if "if (w == NULL || !XmIsScrollBar(w))" not in body:
        raise SystemExit(f"{name} does not reject non-ScrollBar widgets")
    if body.index("if (w == NULL || !XmIsScrollBar(w))") > body.index(
        "XtWidgetToApplicationContext(w)"
    ):
        raise SystemExit(f"{name} validates its widget after context lookup")

print("ScrollBar value APIs validate widget class before access")
