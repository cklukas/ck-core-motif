#!/usr/bin/env python3
"""Check table converters validate source and destination XrmValue inputs."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c").read_text(
    encoding="utf-8"
)
for name in ("CvtStringToXmStringTable(", "CvtStringToStringTable(",
             "CvtStringToCardinalList("):
    start = text.rfind(name)
    body = text[start : start + 1100]
    if "from_val == NULL" not in body or "to_val == NULL" not in body:
        raise SystemExit(f"table converter guard is missing: {name}")

print("table converter input guards present")
