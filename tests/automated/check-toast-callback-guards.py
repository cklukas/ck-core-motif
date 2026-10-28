#!/usr/bin/env python3
"""Ensure Toast callbacks validate their widget data before dereferencing."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Toast.c").read_text(
    encoding="utf-8"
)
for marker, next_marker in (("static void activate(Widget widget", "\nstatic void events"),
                            ("static void timeout(XtPointer data", "\nWidget XmCreateToast")):
    start = source.index(marker)
    body = source[start : source.index(next_marker, start)]
    assert "XmIsToast((Widget)toast)" in body, marker
    assert body.index("XmIsToast((Widget)toast)") < body.index("toast->toast")
print("PASS Toast callbacks validate widget data")
