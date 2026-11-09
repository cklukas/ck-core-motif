#!/usr/bin/env python3
"""Check Skeleton animation never redraws a widget without an X window."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Skeleton.c").read_text(
    encoding="utf-8"
)

tick_start = source.index("static void tick(")
tick_end = source.index("\nWidget XmCreateSkeleton", tick_start)
tick = source[tick_start:tick_end]
clear = "XClearArea(XtDisplay((Widget)skeleton), XtWindow((Widget)skeleton),"
assert clear in tick and "if (XtIsRealized((Widget)skeleton)) " + clear in tick

stop_start = source.index("void XmSkeletonStop(")
stop_end = source.index("\nBoolean XmSkeletonIsAnimating", stop_start)
stop = source[stop_start:stop_end]
clear_stop = "XClearArea(XtDisplay(widget), XtWindow(widget),"
assert clear_stop in stop and "if (XtIsRealized(widget)) " + clear_stop in stop

print("PASS Skeleton animation guards unrealized redraws")
