#!/usr/bin/env python3
"""Check XmPrompt keeps its render table when copying a replacement fails."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/Prompt.c").read_text(
    encoding="utf-8"
)
start = source.index("static Boolean SetValues(Widget old_widget")
end = source.index("static XtGeometryResult QueryGeometry", start)
context = source[start:end]
required = (
    "if (new_font)",
    "prompt->prompt.font = old->prompt.font;",
    "font_changed = False;",
)
missing = [fragment for fragment in required if fragment not in context]
if missing:
    raise SystemExit("XmPrompt render-table rollback missing: " + ", ".join(missing))

print("XmPrompt render-table rollback check passed")
