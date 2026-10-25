#!/usr/bin/env python3
"""Check that Screen pixmap/cache helpers validate screen inputs."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "Screen.c"
text = source.read_text(encoding="utf-8")
for needle in (
    "if (xmScreen == NULL || pixmap == None)",
    "if (scratchTable == NULL || inUseTable == NULL)",
    "if (xmScreen == NULL)\n        return NULL;",
):
    if needle not in text:
        raise SystemExit(f"Screen pixmap/cache input guard is missing: {needle}")

print("Screen pixmap/cache input guards present")
