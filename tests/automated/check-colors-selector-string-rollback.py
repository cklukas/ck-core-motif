#!/usr/bin/env python3
"""Check ColorSelector string resources preserve old values on failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorS.c").read_text()
start = source.index("SetValues(Widget current")
end = source.index("\n    return FALSE;", start)
region = source[start:end]
assert "XmString replacement;" in region
assert region.count("replacement = XmStringCopy") >= 4
assert region.count("if (replacement != NULL)") >= 4
assert "XmColorS_strings(csw).slider_labels[i] =" in region
assert "XmColorS_strings(csw).tog_labels[i] =" in region
assert "XmColorS_strings(csw).file_read_error =" in region
assert "XmColorS_strings(csw).no_cell_error =" in region
assert "XmColorS_strings(curr).slider_labels[i];" in region
print("ColorSelector string resources preserve current values on copy failure")
