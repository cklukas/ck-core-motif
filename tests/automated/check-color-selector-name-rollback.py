#!/usr/bin/env python3
"""Check ColorSelector color-name replacement handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorS.c").read_text()
start = source.index("color_name_changed(XmColorSelectorWidget csw, char *name)")
end = source.index("/*\tFunction Name: SetSliders", start)
region = source[start:end]

assert "String replacement;" in region
assert "replacement = XtNewString(name);" in region
assert "if (replacement == NULL)" in region
assert region.index("if (replacement == NULL)") < region.index("XmColorS_color_name(csw) = replacement;")
assert "XmColorS_color_name(csw) = old_val;" in region
print("ColorSelector preserves color name on copy failure")
