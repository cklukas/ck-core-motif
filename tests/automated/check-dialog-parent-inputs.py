#!/usr/bin/env python3
"""Check dialog shell helpers reject null parents before creation work."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DialogS.c").read_text(
    encoding="utf-8"
)

for signature, lookup in (
    ("XmCreateDialogShell(\n        Widget p,", "XtCreatePopupShell"),
    ("XmeCreateClassDialog(\n\tWidgetClass w_class,", "if (bb_n"),
):
    start = source.index(signature)
    body_start = source.index("{", start)
    body_end = source.index("\n}", body_start)
    body = source[body_start:body_end]
    guard = "if (p == (Widget) NULL)\n        return NULL;" if "XmCreate" in signature else "if (ds_p == (Widget) NULL)\n\treturn NULL;"
    assert guard in body
    assert body.index(guard) < body.index(lookup)

print("dialog shell helpers guard null parents before creation work")
