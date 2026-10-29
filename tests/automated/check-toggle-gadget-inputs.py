#!/usr/bin/env python3
"""Check direct ToggleButton gadget APIs reject NULL widgets."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "ToggleBG.c").read_text(
    encoding="utf-8"
)

for name, result in (
    ("XmToggleButtonGadgetGetState(\n        Widget w )", "return False;"),
    ("XmToggleButtonGadgetSetState(\n        Widget w,", "return;"),
    ("XmToggleButtonGadgetSetValue(\n        Widget w,", "return False;"),
):
    start = source.index(name)
    end = source.index("\n}\n", start)
    region = source[start:end]
    assert "if (w == NULL)" in region, name
    assert result in region, name
    assert region.index("if (w == NULL)") < region.index(
        "XtWidgetToApplicationContext(w)"
    ), name

print("PASS ToggleButton gadget APIs reject NULL widgets")
