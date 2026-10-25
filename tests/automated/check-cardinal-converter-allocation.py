#!/usr/bin/env python3
"""Check Cardinal-list converter allocation and growth guards."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c"
text = source.read_text()
start = text.index("CvtStringToCardinalList(")
end = text.index("static void\nCardinalListCvtDestroy", start)
block = text[start:end]

for needle in (
    "if (crd_array == NULL)",
    "Cardinal *new_array",
    "if (new_array == NULL)",
    "XtFree((char *)crd_array);",
):
    if needle not in block:
        raise SystemExit(f"Cardinal-list converter is missing {needle}")

print("Cardinal-list conversion handles allocation failure")
