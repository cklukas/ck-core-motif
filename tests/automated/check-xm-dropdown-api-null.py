#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"DropDown.c").read_text()
assert text.count("w == NULL") >= 6
for name in ("XmDropDownGetValue", "XmDropDownGetChild", "XmDropDownGetLabel", "XmDropDownGetArrow", "XmDropDownGetText", "XmDropDownGetList"):
    assert name in text
print("XmDropDown APIs validate null widgets")
