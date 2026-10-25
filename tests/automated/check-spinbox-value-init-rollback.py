#!/usr/bin/env python3
"""Check SpinBox value-list initialization rolls back partial copies."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "SpinB.c").read_text()
start = source.index("ConstraintInitialize(Widget req")
end = source.index("\nstatic void \nConstraintDestroy", start)
region = source[start:end]

assert "XmString\t\tcopy;" in region
assert "copy = XmStringCopy(reqC->values[valLoop]);" in region
assert "while (valLoop != 0)" in region
assert "XmStringFree(newC->values[--valLoop]);" in region
assert "XtFree((char *)newC->values);" in region
assert "newC->values = NULL;" in region
assert "newC->num_values = 0;" in region
print("SpinBox value-list initialization rolls back partial copies")
