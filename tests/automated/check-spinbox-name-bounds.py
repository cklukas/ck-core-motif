#!/usr/bin/env python3
"""Check that the SpinBox child name is measured and bounded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "SSpinB.c").read_text()
start = source.index("static void\nInitialize")
end = source.index("\n}\n\n/*ARGSUSED*/", start) + 2
region = source[start:end]
assert "widget_name_length = snprintf(NULL, 0, SSB_TEXTFIELD_NAME_FORMAT" in region
assert "(size_t)widget_name_length >= (size_t)UINT_MAX" in region
assert "XtMalloc((size_t)widget_name_length + 1)" in region
assert "if (widget_name == NULL)" in region
assert "snprintf(widget_name, (size_t)widget_name_length + 1" in region
assert "sprintf(widget_name, SSB_TEXTFIELD_NAME_FORMAT" not in region
print("SpinBox child-name construction is bounded")
