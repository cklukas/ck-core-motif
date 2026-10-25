#!/usr/bin/env python3
"""Check ComboBox default-action dispatch handles item-copy failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ComboBox.c").read_text()
start = source.index("XmString item = GetEditBoxValue")
end = source.index("\n\t  XmStringFree(item);", start)
region = source[start:end]

assert "call_data.item" in region and "XmStringCopy(item)" in region
assert "if (call_data.item != NULL)" in region
assert "XtCallCallbacks(CB_List(cb), XmNdefaultActionCallback" in region
assert region.index("if (call_data.item != NULL)") < region.index("XtCallCallbacks")
print("ComboBox default-action dispatch handles item allocation failure")
