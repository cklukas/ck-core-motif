#!/usr/bin/env python3
"""Check ToggleSwitch label replacement is transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ToggleSwitch.c").read_text()
start = source.index("static Boolean SetValues(Widget current")
end = source.index("static XtGeometryResult QueryGeometry", start)
region = source[start:end]

assert "XmString replacement;" in region
assert "replacement = new_sw->toggle_switch.label != NULL" in region
assert "if (new_sw->toggle_switch.label == NULL || replacement != NULL)" in region
assert "new_sw->toggle_switch.label = old_sw->toggle_switch.label;" in region
assert region.index("XmStringCopy(new_sw->toggle_switch.label)") < region.index("XmStringFree(old_sw->toggle_switch.label)")
print("ToggleSwitch preserves labels on copy failure")
