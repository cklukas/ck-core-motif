#!/usr/bin/env python3
"""Check popup post-list APIs validate the menu widget before access."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RCPopup.c").read_text(
    encoding="utf-8"
)

for name in ("XmAddToPostFromList", "XmRemoveFromPostFromList"):
    start = source.index(name + "(")
    body_start = source.index("{", start)
    body_end = source.index("\n}", body_start)
    body = source[body_start:body_end]
    guard = "if (m == (Widget) NULL)\n    return;"
    assert guard in body
    assert body.index(guard) < body.index("XmRowColumnWidget menu")
    assert body.index(guard) < body.index("_XmWidgetToAppContext(m)")

print("popup post-list APIs guard null menus before context lookup")
