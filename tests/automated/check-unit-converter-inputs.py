#!/usr/bin/env python3
"""Check unit converters validate widget, source, and destination inputs."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c").read_text(
    encoding="utf-8"
)
for name in ("CvtStringToHorizontalPosition(",
             "CvtStringToHorizontalDimension(",
             "CvtStringToVerticalPosition(",
             "CvtStringToVerticalDimension("):
    start = text.rfind(name)
    body = text[start : start + 1000]
    for needle in ("args == NULL", "from == NULL", "from->addr == NULL",
                   "to == NULL", "widget == NULL"):
        if needle not in body:
            raise SystemExit(f"unit converter guard is missing: {needle} ({name})")

print("unit converter input guards present")
