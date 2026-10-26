#!/usr/bin/env python3
"""Check live XmPrompt color changes update its drawing GC."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/Prompt.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean SetValues(Widget old_widget")
end = source.index("static XtGeometryResult QueryGeometry", start)
context = source[start:end]
required = (
    "Boolean colors_changed =",
    "XSetForeground(XtDisplay(widget), prompt->prompt.gc,",
    "XSetBackground(XtDisplay(widget), prompt->prompt.gc,",
    "font_changed || colors_changed",
)
missing = [fragment for fragment in required if fragment not in context]
if missing:
    raise SystemExit("XmPrompt color update missing: " + ", ".join(missing))

print("XmPrompt color-update check passed")
