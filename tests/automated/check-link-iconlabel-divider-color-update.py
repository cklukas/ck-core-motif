#!/usr/bin/env python3
"""Check cached label/divider GCs follow runtime color resources."""
from pathlib import Path

root = Path(__file__).parents[2]
contracts = {
    "Link.c": "link->link.gc",
    "IconLabel.c": "label->icon_label.foreground_gc",
    "Divider.c": "divider->divider.foreground_gc",
}
for name, gc in contracts.items():
    source = (root / "lib" / "Xm" / name).read_text(encoding="utf-8")
    start = source.index("static Boolean SetValues(Widget old_widget")
    end = source.index("static XtGeometryResult QueryGeometry", start)
    body = source[start:end]
    for fragment in (
        "Boolean colors_changed =",
        "if (colors_changed",
        f"XSetForeground(XtDisplay(widget), {gc},",
        f"XSetBackground(XtDisplay(widget), {gc},",
        "colors_changed",
    ):
        if fragment not in body:
            raise SystemExit(f"{name} color update contract missing: {fragment}")

print("Link, IconLabel, and Divider color changes refresh cached drawing GCs")
