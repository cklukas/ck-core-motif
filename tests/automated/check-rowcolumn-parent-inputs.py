#!/usr/bin/env python3
"""Check RowColumn creation wrappers validate parent widgets first."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RowColumn.c").read_text(
    encoding="utf-8"
)

for name in (
    "XmCreateRowColumn",
    "XmCreateWorkArea",
    "XmCreateRadioBox",
    "XmCreateOptionMenu",
    "XmCreateMenuBar",
    "XmCreatePopupMenu",
    "XmCreatePulldownMenu",
):
    start = source.index(name + "(")
    body_start = source.index("{", start)
    body_end = source.index("\n}", body_start)
    body = source[body_start:body_end]
    guard = "if (p == (Widget) NULL)"
    assert guard in body
    assert "return NULL;" in body
    assert body.index(guard) < body.index("_XmWidgetToAppContext(p)")

print("RowColumn creation wrappers guard null parents before context lookup")
