#!/usr/bin/env python3
"""Check public controls reject late input and stale timer delivery."""

from pathlib import Path


root = Path(__file__).parents[2] / "lib" / "Xm"


def body(filename: str, start: str, end: str) -> str:
    source = (root / filename).read_text()
    offset = source.index(start)
    return source[offset:source.index(end, offset + len(start))]


assert "widget->core.being_destroyed" in body(
    "Badge.c", "int XmBadgeGetDisplayedCount", "static Boolean BufferedSetValues")
assert "widget->core.being_destroyed" in body(
    "SegmentedControl.c", "Boolean XmSegmentedControlSetSelectedIndex",
    "void XmSegmentedControlClearSelection")
assert "widget->core.being_destroyed" in body(
    "Link.c", "static void Events", "void XmLinkActivate")
assert "spinner->core.being_destroyed" in body(
    "Spinner.c", "static void Tick", "static void Notify")

tip = (root / "TeachingTip.c").read_text()
assert "tip->core.being_destroyed" in body(
    "TeachingTip.c", "static void show_timeout", "static Boolean SetValues")
assert "owner)->core.being_destroyed" in body(
    "TeachingTip.c", "void XmTeachingTipShow", "void XmTeachingTipHide")
assert "popup_shell)->core.being_destroyed" in body(
    "TeachingTip.c", "void XmTeachingTipHide", "Boolean XmTeachingTipIsVisible")

toast = (root / "Toast.c").read_text()
assert "static void restart_timeout" in toast
assert "toast->toast.timer = (XtIntervalId)0" in body(
    "Toast.c", "static void restart_timeout", "static void preferred")
assert "restart_timeout(toast)" in body(
    "Toast.c", "static Boolean SetValues(Widget old_widget",
    "static XtGeometryResult GeometryManager")
assert "toast->toast.timer = (XtIntervalId)0" in body(
    "Toast.c", "void XmToastShow", "void XmToastDismiss")
assert "widget->core.being_destroyed" in body(
    "Toast.c", "static void events(Widget widget", "static void timeout")

print("PASS control timer and teardown entry points are guarded")
