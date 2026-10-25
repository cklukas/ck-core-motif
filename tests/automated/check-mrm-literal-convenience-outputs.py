#!/usr/bin/env python3
"""Check public Mrm literal convenience output contracts."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmlread.c").read_text(encoding="utf-8")
checks = {
    "MrmFetchIconLiteral (MrmHierarchy": (
        "if ( pixmap_return == NULL )",
        "*pixmap_return = 0;",
    ),
    "MrmFetchBitmapLiteral (MrmHierarchy": (
        "if ( pixmap_return == NULL || width == NULL || height == NULL )",
        "*pixmap_return = 0;",
        "*width = 0;",
        "*height = 0;",
    ),
    "MrmFetchColorLiteral (MrmHierarchy": (
        "if ( pixel_return == NULL )",
        "*pixel_return = 0;",
    ),
}
for signature, needles in checks.items():
    start = source.index(signature)
    end = source.find("\nMrmFetch", start + 1)
    body = source[start:] if end < 0 else source[start:end]
    lock = body.index("_MrmDisplayToAppContext")
    for needle in needles:
        if needle not in body or body.index(needle) > lock:
            raise SystemExit(f"{signature} does not initialize {needle}")
print("PASS Mrm literal convenience APIs validate and initialize their outputs")
