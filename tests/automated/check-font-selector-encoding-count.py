#!/usr/bin/env python3
"""Check Font Selector encoding-list counting and allocation."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/FontS.c").read_text(
    encoding="utf-8"
)

assert "SIZE_MAX / sizeof(*newList)" in source
assert "UINT_MAX / sizeof(*newList)" in source
assert "XtMalloc((Cardinal)((size_t)i * sizeof(*newList)))" in source
assert "*encodings != NULL; i++, encodings++) \n\t\ti++;" not in source
print("Font Selector encoding count is single-step and bounded")
