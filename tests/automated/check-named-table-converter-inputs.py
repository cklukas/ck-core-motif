#!/usr/bin/env python3
"""Check named table converters and destructors validate Xt values."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c").read_text(
    encoding="utf-8"
)
for name in ("ConvertStringToButtonType(", "CvtStringToKeySymTable(",
             "CvtStringToCharSetTable("):
    start = text.rfind(name)
    body = text[start : start + 900]
    if "from == NULL" not in body or "to == NULL" not in body:
        raise SystemExit(f"named table converter guard is missing: {name}")
for name in ("ConvertStringToButtonTypeDestroy(",
             "CvtStringToKeySymTableDestroy(",
             "CvtStringToCharSetTableDestroy("):
    start = text.rfind(name)
    body = text[start : start + 400]
    if "to == NULL || to->addr == NULL" not in body:
        raise SystemExit(f"named table destructor guard is missing: {name}")

print("named table converter guards present")
