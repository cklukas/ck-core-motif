#!/usr/bin/env python3
"""Check LabelGadget SetValues preserves strings on copy failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "LabelG.c").read_text()
start = source.index("static Boolean\nSetValues(Widget cw")
end = source.index("\n/************************************************************************", start)
region = source[start:end]

assert "XmString replacement;" in region
assert "replacement = XmStringCopy((XmString) LabG__label(new_w));" in region
assert "LabG__label(new_w) = LabG__label(current);" in region
assert "replacement = XmStringCopy(LabG__acceleratorText(new_w));" in region
assert "LabG__acceleratorText(new_w) =\n                            LabG__acceleratorText(current);" in region
assert "if (LabG__label(new_w) != LabG__label(current))" in region
assert "if (LabG__acceleratorText(new_w) !=" in region
print("LabelGadget SetValues preserves strings on allocation failure")
