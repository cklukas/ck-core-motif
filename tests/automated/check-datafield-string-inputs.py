#!/usr/bin/env python3
"""Check DataField string getters validate widgets before Xt lookup."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text(
    encoding="utf-8"
)

for name, widget, result in (
    ("XmDataFieldGetString(\n        Widget w )", "w", "return NULL;"),
    ("XmDataFieldGetSubstring(\n\tWidget widget,", "widget", "return XmCOPY_FAILED;"),
    ("XmDataFieldGetStringWcs(\n        Widget w )", "w", "return NULL;"),
    ("XmDataFieldGetSubstringWcs(\n\tWidget widget,", "widget", "return XmCOPY_FAILED;"),
):
    start = source.index(name)
    end = source.index("\n}\n", start)
    region = source[start:end]
    guard = f"if ({widget} == NULL)"
    lookup = f"_XmWidgetToAppContext({widget})"
    assert guard in region, name
    assert result in region, name
    assert region.index(guard) < region.index(lookup), name

print("PASS DataField string APIs validate widgets before context lookup")
