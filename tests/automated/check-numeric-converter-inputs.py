#!/usr/bin/env python3
"""Check numeric resource converters validate XrmValue and widget inputs."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c").read_text(
    encoding="utf-8"
)
for name in ("CvtStringToBooleanDimension(", "CvtStringToCardinal(",
             "CvtStringToTextPosition(", "CvtStringToTopItemPosition("):
    start = text.rfind(name)
    body = text[start : start + 900]
    if "from == NULL" not in body or "to == NULL" not in body:
        raise SystemExit(f"numeric converter guard is missing: {name}")
if "args == NULL || args[0].addr == NULL" not in text:
    raise SystemExit("boolean-dimension widget argument guard is missing")

print("numeric converter input guards present")
