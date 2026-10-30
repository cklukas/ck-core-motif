#!/usr/bin/env python3
"""Check Calendar and TreeView cached text GCs follow color resources."""
from pathlib import Path

root = Path(__file__).parents[2]
contracts = {
    "Calendar.c": "calendar->calendar.gc",
    "TreeView.c": "view->tree_view.gc",
}
for name, gc in contracts.items():
    source = (root / "lib" / "Xm" / name).read_text(encoding="utf-8")
    start = source.index("static Boolean SetValues(Widget old_widget")
    end = source.index("static XtGeometryResult QueryGeometry", start)
    body = source[start:end]
    for fragment in (
        "Boolean colors_changed =",
        "if (colors_changed",
        f"XSetForeground(XtDisplay(",
        f"{gc},",
        "XSetBackground(",
        "colors_changed",
    ):
        if fragment not in body:
            raise SystemExit(f"{name} color update contract missing: {fragment}")

print("Calendar and TreeView color changes refresh cached text GCs")
