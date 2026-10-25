#!/usr/bin/env python3
"""Check SpinBox SetValues rolls back replacement value-list copies."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "SpinB.c").read_text()
start = source.index("ConstraintSetValues(Widget")
end = source.index("\n  /* If the current focus child", start)
region = source[start:end]

assert "XmString\t       copy;" in region
assert "if (reqC->num_values > 0 && newC->values == NULL)" in region
assert "while (valLoop != 0)" in region
assert "XmStringFree(newC->values[--valLoop]);" in region
assert "newC->values = oldC->values;" in region
assert "newC->num_values = oldC->num_values;" in region
assert "reqC->values = oldC->values;" in region
assert "SIZE_MAX / sizeof(XmString)" in region
assert "UINT_MAX / sizeof(XmString)" in region
print("SpinBox SetValues preserves value lists on allocation failure")
