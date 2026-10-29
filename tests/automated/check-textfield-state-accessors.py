#!/usr/bin/env python3
"""Check TextField state accessors reject invalid inputs before Xt lookup."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text(
    encoding="utf-8"
)

expected = {
    "XmTextFieldGetAddMode(Widget w)": "if (w == NULL)",
    "XmTextFieldGetEditable(Widget w)": "if (w == NULL)",
    "XmTextFieldSetEditable(Widget w,": "if (w == NULL)",
    "XmTextFieldGetMaxLength(Widget w)": "if (w == NULL)",
    "XmTextFieldSetMaxLength(Widget w,": "if (w == NULL)",
    "XmTextFieldGetInsertionPosition(Widget w)": "if (w == NULL)",
    "XmTextFieldSetInsertionPosition(Widget w,": "if (w == NULL)",
    "XmTextFieldGetSelection(Widget w)": "if (w == NULL)",
    "XmTextFieldGetSelectionWcs(Widget w)": "if (w == NULL)",
}

for name, guard in expected.items():
    start = source.index(name)
    end = source.index("\n}\n", start)
    region = source[start:end]
    assert guard in region, name
    assert region.index(guard) < region.index("_XmWidgetToAppContext(w)"), name

selection_start = source.index("XmTextFieldGetSelectionPosition(Widget w,")
selection_end = source.index("\n}\n", selection_start)
selection = source[selection_start:selection_end]
assert "if (w == NULL || left == NULL || right == NULL)" in selection

print("PASS TextField state accessors validate widgets and outputs")
