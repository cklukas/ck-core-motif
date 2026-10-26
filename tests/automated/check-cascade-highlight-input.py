#!/usr/bin/env python3
"""Check cascade-gadget highlighting rejects a null widget before context use."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CascadeBG.c").read_text(
    encoding="utf-8"
)
start = source.index("XmCascadeButtonGadgetHighlight(")
body_start = source.index("{", start)
body_end = source.index("\n}", body_start)
body = source[body_start:body_end]
guard = "if (wid == (Widget) NULL)\n      return;"
assert guard in body
assert body.index(guard) < body.index("_XmWidgetToAppContext(wid)")
print("cascade-gadget highlight guards null widgets before context lookup")
