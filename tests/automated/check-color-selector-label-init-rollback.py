#!/usr/bin/env python3
"""Check ColorSelector initialization preserves labels on copy failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorS.c").read_text()
start = source.index("    {\n    int i;", source.index("Initialize("))
end = source.index("\n    }\n\n}", start)
region = source[start:end]

assert "XmString replacement;" in region
assert region.count("replacement = XmStringCopy") == 4
assert region.count("if (replacement != NULL)") == 4
assert "= XmStringCopy(XmColorS_strings(csw).slider_labels[i]);" in region
assert "= XmStringCopy(XmColorS_strings(csw).tog_labels[i]);" in region
assert "= XmStringCopy(XmColorS_strings(csw).file_read_error);" in region
assert "= XmStringCopy(XmColorS_strings(csw).no_cell_error);" in region
assert "= XmStringCopy(XmColorS_strings(csw).slider_labels[i]);\n    for" not in region
print("ColorSelector initialization preserves labels on allocation failure")
