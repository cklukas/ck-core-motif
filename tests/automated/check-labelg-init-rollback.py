#!/usr/bin/env python3
"""Check LabelGadget initialization preserves strings on copy failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "LabelG.c").read_text()
start = source.index("static void\nInitialize(", source.index("Initialize(Widget req"))
end = source.index("\n/************************************************************************", start)
region = source[start:end]

assert "XmString replacement;" in region
label_copy = "replacement = XmStringCopy((XmString) LabG__label(new_w));"
acc_copy = "replacement = XmStringCopy(\n                    (XmString) LabG__acceleratorText(new_w));"
assert label_copy in region
assert acc_copy in region
assert region.count("if (replacement != NULL)") >= 2
assert "XmStringCreateLocalized(\n                XrmQuarkToString(lw->object.xrm_name))" in region
assert "LabG__acceleratorText(new_w) = replacement;" in region
print("LabelGadget initialization preserves strings on allocation failure")
