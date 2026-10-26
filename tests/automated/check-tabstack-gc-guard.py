#!/usr/bin/env python3
"""Check that TabStack rendering tolerates GC creation failure."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "TabStack.c"
text = source.read_text()

for needle in (
    "XmTabStack__gc(tab) = XCreateGC",
    "if (XmTabStack__gc(tab) == NULL)",
):
    if needle not in text:
        raise SystemExit(f"TabStack GC handling is missing {needle}")

if text.count("if (XmTabStack__gc(tab) == NULL)") < 2:
    raise SystemExit("TabStack resize and redisplay paths are not both guarded")

print("TabStack rendering guards failed GC creation")
