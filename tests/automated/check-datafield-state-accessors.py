#!/usr/bin/env python3
"""Check DataField state and selection APIs validate inputs before Xt lookup."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text(
    encoding="utf-8"
)

for name, result in (
    ("XmDataFieldSetAddMode(\n        Widget w,", "return;"),
    ("XmDataFieldGetAddMode(\n        Widget w )", "return False;"),
    ("XmDataFieldGetEditable(\n        Widget w )", "return False;"),
    ("XmDataFieldSetEditable(\n        Widget w,", "return;"),
    ("XmDataFieldGetMaxLength(\n        Widget w )", "return 0;"),
    ("XmDataFieldSetMaxLength(\n        Widget w,", "return;"),
    ("XmDataFieldGetCursorPosition(\n        Widget w )", "return 0;"),
    ("XmDataFieldGetInsertionPosition(\n        Widget w )", "return 0;"),
    ("XmDataFielddf_SetCursorPosition(\n        Widget w,", "return;"),
    ("XmDataFieldSetInsertionPosition(\n        Widget w,", "return;"),
    ("XmDataFieldGetSelection(\n        Widget w )", "return NULL;"),
    ("XmDataFieldGetSelectionWcs(\n        Widget w )", "return NULL;"),
):
    start = source.index(name)
    end = source.index("\n}\n", start)
    region = source[start:end]
    assert "if (w == NULL)" in region, name
    assert result in region, name
    assert region.index("if (w == NULL)") < region.index(
        "_XmWidgetToAppContext(w)"
    ), name

selection_start = source.index("XmDataFieldGetSelectionPosition(\n        Widget w,")
selection_end = source.index("\n}\n", selection_start)
selection = source[selection_start:selection_end]
assert "if (w == NULL || left == NULL || right == NULL)" in selection

print("PASS DataField state and selection APIs validate inputs")
