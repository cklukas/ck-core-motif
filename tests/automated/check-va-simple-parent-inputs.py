#!/usr/bin/env python3
"""Check varargs SimpleMenu constructors validate parents before access."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "VaSimple.c").read_text(
    encoding="utf-8"
)

for name in (
    "XmVaCreateSimpleMenuBar",
    "XmVaCreateSimplePulldownMenu",
    "XmVaCreateSimplePopupMenu",
    "XmVaCreateSimpleOptionMenu",
):
    start = source.index(name + "(")
    body_start = source.index("{", start)
    body_end = source.index("\n}", body_start)
    body = source[body_start:body_end]
    guard = "if (parent == (Widget) NULL)\n\treturn NULL;"
    assert guard in body
    assert body.index(guard) < body.index("_XmWidgetToAppContext(parent)")

print("varargs SimpleMenu constructors guard null parents before context lookup")
