#!/usr/bin/env python3
"""Check ColorSelector RGB-file replacement is transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorS.c").read_text()
start = source.index("SetValues(Widget current")
end = source.index("if ((XmColorS_margin_height", start)
region = source[start:end]

assert "String rgb_file_copy;" in region
assert "Boolean rgb_file_changed;" in region
assert "rgb_file_copy = XmColorS_rgb_file(csw) != NULL" in region
assert "if (XmColorS_rgb_file(csw) == NULL || rgb_file_copy != NULL)" in region
assert "XmColorS_rgb_file(csw) = XmColorS_rgb_file(curr);" in region
assert region.index("XtNewString(XmColorS_rgb_file(csw))") < region.index("XtFree((char*) XmColorS_rgb_file(curr))")
assert region.index("read_rgb_file(csw, NULL, 0, False)") > region.index("XmColorS_rgb_file(csw) = rgb_file_copy;")
print("ColorSelector preserves RGB-file state on copy failure")
