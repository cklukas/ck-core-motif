#!/usr/bin/env python3
"""Check TextField mutators reject NULL widgets before Xt lookup."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text(
    encoding="utf-8"
)

for name in (
    "XmTextFieldSetString(Widget w,",
    "XmTextFieldSetStringWcs(Widget w,",
    "XmTextFieldReplace(Widget w,",
    "XmTextFieldReplaceWcs(Widget w,",
    "XmTextFieldInsert(Widget w,",
    "XmTextFieldInsertWcs(Widget w,",
    "XmTextFieldSetAddMode(Widget w,",
):
    start = source.index(name)
    end = source.index("\n}\n", start)
    region = source[start:end]
    assert "if (w == NULL)" in region, name
    assert region.index("if (w == NULL)") < region.index(
        "_XmWidgetToAppContext(w)"
    ), name

print("PASS TextField mutators reject NULL widgets before context lookup")
