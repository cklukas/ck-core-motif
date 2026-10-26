#!/usr/bin/env python3
"""Check input validation in the shared MWM bevel primitives."""

from pathlib import Path


source = Path("clients/mwm/WmGraphics.c").read_text(encoding="utf-8")

for marker, extra in (
    ("void BevelRectangle (RList *prTop", "right_wid > width - left_wid"),
    ("void BevelDepressedRectangle (RList *prTop", "in_wid > left_wid"),
):
    start = source.index(marker)
    end = source.index("} /* END OF FUNCTION", start)
    body = source[start:end]
    assert "prTop == NULL || prBot == NULL" in body
    assert "width == 0 || height == 0" in body
    assert "top_wid > UINT_MAX - left_wid" in body
    assert extra in body
    guard = body.index("if (prTop == NULL")
    assert guard < body.index("prTop->used")

print("PASS MWM bevel primitives reject invalid geometry")
