#!/usr/bin/env python3
"""Ensure SegmentedControl child-key dispatch validates its closure type."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "SegmentedControl.c").read_text(
    encoding="utf-8"
)
start = source.index("static void child_key(Widget child")
body = source[start : source.index("\nstatic Boolean SetValues", start)]
guard = "!XmIsSegmentedControl((Widget)control)"
assert guard in body
assert body.index(guard) < body.index("event->type")
print("PASS SegmentedControl child-key callbacks validate closure type")
