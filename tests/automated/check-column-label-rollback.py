#!/usr/bin/env python3
"""Check Column constraint labels preserve current values on copy failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Column.c").read_text()
start = source.index("ConstraintSetValues(Widget current")
end = source.index("\n    if( cc->label_pixmap", start)
region = source[start:end]
assert "XmString               replacement;" in region
assert "replacement = XmStringCopy(sc->label_string);" in region
assert "if (replacement != NULL)" in region
assert "XmStringFree(cc->label_string);" in region
assert "sc->label_string = cc->label_string;" in region
print("Column labels preserve the current value on copy failure")
