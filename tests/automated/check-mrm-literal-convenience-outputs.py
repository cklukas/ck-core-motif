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
    for needle in needles:
        assert needle in body, (signature, needle)
        assert body.index(needle) < body.index("_MrmDisplayToAppContext")

bitmap_start = source.index("MrmFetchBitmapLiteral (MrmHierarchy")
bitmap_end = source.index("MrmFetchColorLiteral (MrmHierarchy", bitmap_start)
bitmap = source[bitmap_start:bitmap_end]
conversion = bitmap.index("result = UrmCreateBitmap")
dimensions = bitmap.index("*width = icon->width;", conversion)
assert "if ( result == MrmSUCCESS )" in bitmap[conversion:dimensions]
assert "*height = icon->height;" in bitmap[dimensions:]
print("Mrm literal convenience APIs validate and initialize their outputs")
