#!/usr/bin/env python3
"""Check that BaseClass wrapper setup handles private-record allocation failure."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "BaseClass.c"
text = source.read_text()

for needle in (
    "if (wcData == NULL || scData == NULL)",
    "if (wrapperData == NULL)",
):
    if needle not in text:
        raise SystemExit(f"BaseClass.c is missing {needle}")

print("BaseClass wrapper setup handles private-record allocation failure")
