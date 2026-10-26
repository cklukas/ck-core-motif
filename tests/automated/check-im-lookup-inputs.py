#!/usr/bin/env python3
"""Check XmImMbLookupString validates widget, event, and buffer inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text(encoding="utf-8")
start = source.index("XmImMbLookupString(")
region = source[start:]
assert "if (w == NULL || event == NULL || nbytes < 0" in region
assert "(nbytes != 0 && buf == NULL)" in region
assert region.index("if (w == NULL || event == NULL || nbytes < 0") < region.index("_XmWidgetToAppContext(w)")
print("PASS XmImMbLookupString validates input arguments")
