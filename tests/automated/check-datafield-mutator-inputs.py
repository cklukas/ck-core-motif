#!/usr/bin/env python3
"""Check DataField mutators reject NULL widgets before Xt lookup."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text(
    encoding="utf-8"
)

for name in (
    "XmDataFieldGetLastPosition(\n        Widget w )",
    "XmDataFieldSetString(\n        Widget w,",
    "XmDataFieldReplace(\n        Widget w,",
    "XmDataFieldReplaceWcs(\n        Widget w,",
    "XmDataFieldInsert(\n        Widget w,",
    "XmDataFieldInsertWcs(\n        Widget w,",
):
    start = source.index(name)
    end = source.index("\n}\n", start)
    region = source[start:end]
    assert "if (w == NULL)" in region, name
    assert region.index("if (w == NULL)") < region.index(
        "_XmWidgetToAppContext(w)"
    ), name

print("PASS DataField mutators reject NULL widgets before context lookup")
