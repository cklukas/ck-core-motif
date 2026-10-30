#!/usr/bin/env python3
"""Check picker color resources refresh their live drawing GCs."""
from pathlib import Path

root = Path(__file__).parents[2]
for name, member in (("DatePicker.c", "date_picker"), ("TimePicker.c", "time_picker")):
    source = (root / "lib" / "Xm" / name).read_text(encoding="utf-8")
    start = source.index("static Boolean SetValues(Widget old_widget")
    end = source.index("static XtGeometryResult QueryGeometry", start)
    body = source[start:end]
    required = (
        "Boolean colors_changed =",
        f"if (colors_changed && picker->{member}.gc)",
        f"XSetForeground(XtDisplay(new_widget), picker->{member}.gc,",
        f"XSetBackground(XtDisplay(new_widget), picker->{member}.gc,",
        "return colors_changed ||",
    )
    missing = [fragment for fragment in required if fragment not in body]
    if missing:
        raise SystemExit(f"{name} color update contract missing: {', '.join(missing)}")

print("DatePicker and TimePicker color changes refresh live drawing GCs")
