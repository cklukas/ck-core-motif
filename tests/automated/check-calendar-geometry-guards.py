#!/usr/bin/env python3
"""Check XmCalendar handles missing GCs and narrow geometry."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Calendar.c").read_text()
draw = source[source.index("static void draw_day"):source.rindex("static void Redisplay(")]
start = source.rindex("static void Redisplay(")
end = source.index("static Boolean month_allowed(", start)
redisplay = source[start:end]
events_start = source.rindex("static void Events(")
events = source[events_start:source.index("static XtGeometryResult QueryGeometry(", events_start)]

assert "!calendar->calendar.gc || cell_width < 1" in draw
assert "cell_width > 5 ? cell_width - 5 : 1" in draw
assert "!calendar->calendar.gc" in redisplay
assert "calendar->core.width < 7" in redisplay
assert "if (cell_width < 1)" in events
notify = source[source.index("static void notify_date("):source.index("static Boolean date_in_range(")]
assert "strcmp(callback_name, XmNcalendarActivateCallback) == 0" in notify
print("PASS Calendar geometry guards")
