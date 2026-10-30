#!/usr/bin/env python3
"""Check Calendar preserves its render table when replacement copy fails."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Calendar.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean SetValues(")
start = source.index("static Boolean SetValues(", start + 1)
end = source.index("\nstatic int first_weekday", start)
region = source[start:end]

if "if (replacement)" not in region:
    raise SystemExit("Calendar does not validate render-table replacement")
if "calendar->calendar.render_table = old_calendar->calendar.render_table;" not in region:
    raise SystemExit("Calendar does not preserve its old render table")
if "XmRenderTableFree(old_calendar->calendar.render_table);" not in region:
    raise SystemExit("Calendar replacement path does not release the old table")

print("PASS Calendar render-table updates are transactional")
