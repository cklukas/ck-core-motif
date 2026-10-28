#!/usr/bin/env python3
"""Ensure SegmentedControl child-key dispatch validates its closure type."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "SegmentedControl.c").read_text(
    encoding="utf-8"
)
guard = "!XmIsSegmentedControl((Widget)control)"
for marker, before in (("static void child_changed(Widget child", "control->segmented.rebuilding"),
                       ("static void child_key(Widget child", "event->type")):
    start = source.index(marker)
    end = source.index("\nstatic ", start + len(marker))
    body = source[start:end]
    assert guard in body
    assert body.index(guard) < body.index(before)
print("PASS SegmentedControl child callbacks validate closure type")
