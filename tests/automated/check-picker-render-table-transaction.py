#!/usr/bin/env python3
"""Check DatePicker and TimePicker preserve render tables on copy failure."""

from pathlib import Path


root = Path(__file__).parents[2] / "lib" / "Xm"
for filename, field in (("DatePicker.c", "date_picker"), ("TimePicker.c", "time_picker")):
    source = (root / filename).read_text(encoding="utf-8")
    start = source.index("static Boolean SetValues(")
    start = source.index("static Boolean SetValues(", start + 1)
    end = source.index("\nstatic void Redisplay", start)
    region = source[start:end]
    if "if (replacement)" not in region:
        raise SystemExit(f"{filename} does not validate render-table replacement")
    if f"picker->{field}.render_table = old->{field}.render_table;" not in region:
        raise SystemExit(f"{filename} does not preserve its old render table")
    if "render_changed = False;" not in region:
        raise SystemExit(f"{filename} reports a failed render-table replacement as changed")

print("PASS DatePicker and TimePicker render-table updates are transactional")
