#!/usr/bin/env python3
"""Check XmScrollVisible validates both widget inputs before access."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScroVis.c").read_text(
    encoding="utf-8"
)
start = source.index("XmScrollVisible(")
body_start = source.index("{", start)
body_end = source.index("\n}", body_start)
body = source[body_start:body_end]
guard = "if (scrw == (Widget) NULL || wid == (Widget) NULL)\n        return;"
assert guard in body
assert body.index(guard) < body.index("sw = (XmScrolledWindowWidget) scrw")
assert body.index(guard) < body.index("_XmWidgetToAppContext(scrw)")
print("scroll-visible guards null scrolled-window and child widgets before access")
