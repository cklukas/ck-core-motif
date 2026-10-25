#!/usr/bin/env python3
"""Check equal ColorSelector names are copied transactionally."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorS.c").read_text()
start = source.index("SetValues(Widget current")
end = source.index("\n    {\n    int i;", start)
region = source[start:end]

assert "String color_name_copy;" in region
assert "color_name_copy = newValue != NULL ? XtNewString(newValue) : NULL;" in region
assert "if (newValue == NULL || color_name_copy != NULL)" in region
assert "XmColorS_color_name(csw) = oldValue;" in region
assert region.index("XtNewString(newValue)") < region.index("XtFree(oldValue)")
print("ColorSelector preserves equal color names on copy failure")
