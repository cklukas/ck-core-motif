#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/Spinner.c").read_text()
start = source.index("static Boolean SetValues(Widget old_widget")
end = source.index("\nstatic XtGeometryResult QueryGeometry", start)
region = source[start:end]
assert "old->spinner.interval != spinner->spinner.interval" in region
assert "XtRemoveTimeOut(spinner->spinner.timer);" in region
assert "spinner->spinner.timer_pending = False;" in region
assert "else if (old->spinner.interval != spinner->spinner.interval)" in region
assert "SyncTimer(spinner);" in region
print("Spinner interval reschedule present")
