#!/usr/bin/env python3
"""Check tooltip replacement preserves the current value on copy failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ToolTip.c").read_text()
start = source.index("XmSetToolTipString (Widget w,")
end = source.index("\n}", start) + 2
region = source[start:end]
assert "XmString replacement;" in region
assert "replacement = s != NULL ? XmStringCopy(s) : NULL;" in region
assert "if (s != NULL && replacement == NULL)" in region
assert "return;" in region
assert "XmStringFree(ttp->tool_tip_string);" in region
assert "ttp->tool_tip_string = replacement;" in region
print("ToolTip strings preserve the current value on copy failure")
