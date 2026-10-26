#!/usr/bin/env python3
"""Check XmPrompt geometry accounts for the rendered text height."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/Prompt.c").read_text(
    encoding="utf-8"
)
start = source.index("static XtGeometryResult QueryGeometry")
end = source.index("void XmPromptSetVisible", start)
context = source[start:end]
required = (
    "reply->height = height > (Dimension)-1 - 8 ? (Dimension)-1 : height + 8;",
    "reply->height = 24;",
)
missing = [fragment for fragment in required if fragment not in context]
if missing:
    raise SystemExit("XmPrompt geometry height handling missing: " + ", ".join(missing))

print("XmPrompt geometry check passed")
