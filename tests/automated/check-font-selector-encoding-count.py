#!/usr/bin/env python3
"""Check Font Selector encoding-list counting and allocation."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/FontS.c").read_text(
    encoding="utf-8"
)

assert "(Cardinal)(UINT_MAX / sizeof(*newList)) - 1" in source
assert "if (max_entries > (Cardinal)INT_MAX - 1)" in source
assert "if (count >= max_entries)" in source
assert "XtCalloc(count + 1, (Cardinal)sizeof(*newList))" in source
assert "*encodings != NULL; i++, encodings++) \n\t\ti++;" not in source
print("Font Selector encoding count is single-step and bounded")
