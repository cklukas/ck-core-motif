#!/usr/bin/env python3
"""Check render-table converters validate display, widget, and XrmValue inputs."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c").read_text(
    encoding="utf-8"
)
start = text.index("cvtStringToXmRenderTable(")
body = text[start : start + 1200]
for needle in ("dpy == NULL", "widget == NULL", "from == NULL", "to == NULL"):
    if needle not in body:
        raise SystemExit(f"render helper guard is missing: {needle}")
for name in ("CvtStringToRenderTable(", "CvtStringToButtonRenderTable(",
             "CvtStringToLabelRenderTable(", "CvtStringToTextRenderTable("):
    start = text.rfind(name)
    body = text[start : start + 800]
    if "args == NULL" not in body or "wid == NULL" not in body:
        raise SystemExit(f"render wrapper guard is missing: {name}")

print("render-table converter input guards present")
