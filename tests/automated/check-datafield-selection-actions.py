#!/usr/bin/env python3
"""Check DataField selection/clipboard actions reject NULL widgets early."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text(
    encoding="utf-8"
)

for name in (
    "XmDataFieldRemove(\n        Widget w )",
    "XmDataFieldCopy(\n        Widget w,",
    "XmDataFieldCut(\n        Widget w,",
    "XmDataFieldPaste(\n        Widget w )",
):
    start = source.index(name)
    end = source.index("\n}\n", start)
    region = source[start:end]
    assert "if (w == NULL)" in region, name
    assert "return False;" in region, name
    assert region.index("if (w == NULL)") < region.index(
        "_XmWidgetToAppContext(w)"
    ), name

copy_start = source.index("XmDataFieldCopy(\n        Widget w,")
copy_end = source.index("\n}\n", copy_start)
copy = source[copy_start:copy_end]
assert copy.index("if (w == NULL)") < copy.index("XtDisplay(w)")
assert "selected_string = XmDataFieldGetSelection(w);" in copy

print("PASS DataField selection actions validate NULL widgets before display use")
