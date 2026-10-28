#!/usr/bin/env python3
"""Ensure EmptyState action callbacks validate their closure type."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "EmptyState.c").read_text(
    encoding="utf-8"
)
start = source.index("static void activate(Widget widget")
body = source[start : source.index("\nWidget XmCreateEmptyState", start)]
guard = "!XmIsEmptyState((Widget)state)"
assert guard in body
assert body.index(guard) < body.index("callback.reason")
print("PASS EmptyState action callbacks validate closure type")
