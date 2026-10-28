#!/usr/bin/env python3
"""Check XmeDragSource rejects a missing initiating event before setup."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmeDragSource(Widget w")
end = source.index("/* Destination section", start)
region = source[start:end]

assert "if (w == NULL || event == NULL)" in region
guard = region.index("if (w == NULL || event == NULL)")
assert region.index("return NULL;", guard) < region.index("_XmWidgetToAppContext(w)")

print("PASS drag source rejects a missing initiating event before setup")
