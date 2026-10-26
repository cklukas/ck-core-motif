#!/usr/bin/env python3
"""Check option-menu gadget lookups validate their menu widget first."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RowColumn.c").read_text(
    encoding="utf-8"
)

for signature in ("XmOptionLabelGadget(\n        Widget m )", "XmOptionButtonGadget(\n        Widget m )"):
    start = source.index(signature)
    body_start = source.index("{", start)
    body_end = source.index("\n}", body_start)
    body = source[body_start:body_end]
    guard = "if (m == (Widget) NULL)\n      return NULL;"
    assert guard in body
    assert body.index(guard) < body.index("_XmWidgetToAppContext(m)")

print("option-menu gadget lookups guard null menu widgets before context lookup")
