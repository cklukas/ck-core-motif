#!/usr/bin/env python3
"""Check XmScaleSetTicks validates its widget before context lookup."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScalTics.c").read_text(
    encoding="utf-8"
)
start = source.index("XmScaleSetTicks(")
body_start = source.index("{", start)
body_end = source.index("\n}", body_start)
body = source[body_start:body_end]
guard = "if (scale == (Widget) NULL)\n        return;"
assert guard in body
assert body.index(guard) < body.index("_XmWidgetToAppContext(scale)")
print("scale tick generation guards null widgets before context lookup")
