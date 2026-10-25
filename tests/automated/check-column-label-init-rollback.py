#!/usr/bin/env python3
"""Check Column constraint-label initialization handles copy failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Column.c").read_text()
start = source.index("static void\nConstraintInitialize")
end = source.index("\n/*\n * Function:\n *\tConstraintSetValues", start)
region = source[start:end]

assert "XmString label_copy;" in region
assert "label_copy = XiC(new_w)->label_string != NULL" in region
assert "XmStringCopy(XiC(new_w)->label_string)" in region
assert "if (XiC(new_w)->label_string != NULL && label_copy == NULL)" in region
assert "label_copy = XmStringCreateLocalized(\"\");" in region
assert "XiC(new_w)->label_string = label_copy;" in region
print("Column constraint labels handle initialization allocation failure")
