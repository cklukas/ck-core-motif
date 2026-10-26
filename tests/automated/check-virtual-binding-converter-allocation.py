#!/usr/bin/env python3
"""Check that the virtual-binding converter handles output allocation failure."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "VirtKeys.c"
text = source.read_text()
start = text.index("CvtStringToVirtualBinding(")
end = text.index("\n}\n", start) + 3
block = text[start:end]

for needle in (
    "toVal->addr = XtMalloc(toVal->size);",
    "if (toVal->addr == NULL)",
    "toVal->size = 0;",
    "return False;",
):
    if needle not in block:
        raise SystemExit(f"virtual-binding converter is missing {needle}")

print("virtual-binding conversion handles output allocation failure")
