#!/usr/bin/env python3
"""Check SimpleMenu creation wrappers validate parents before access."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Simple.c").read_text(
    encoding="utf-8"
)

for name in (
    "XmCreateSimpleMenuBar",
    "XmCreateSimplePopupMenu",
    "XmCreateSimplePulldownMenu",
    "XmCreateSimpleOptionMenu",
):
    start = source.index(name + "(")
    body_start = source.index("{", start)
    body_end = source.index("\n}", body_start)
    body = source[body_start:body_end]
    guard = "if (parent == (Widget) NULL)\n\t    return NULL;"
    assert guard in body
    assert body.index(guard) < body.index("_XmWidgetToAppContext(parent)")

print("SimpleMenu creation wrappers guard null parents before context lookup")
