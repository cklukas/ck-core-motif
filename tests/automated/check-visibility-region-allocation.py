#!/usr/bin/env python3
"""Check that XmGetVisibility handles region allocation failure."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "Traversal.c").read_text(
    encoding="utf-8"
)
start = text.rfind("XmGetVisibility(")
body = text[start : start + 3000]
if "region == NULL || tmp_region == NULL || left_region == NULL" not in body:
    raise SystemExit("visibility region allocation guard is missing")
if "return XmVISIBILITY_FULLY_OBSCURED;" not in body:
    raise SystemExit("visibility allocation fallback is missing")

print("visibility region allocation guard present")
