#!/usr/bin/env python3
"""Check that drag export-target conversion results are type-safe."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmeDragSource(Widget w")
end = source.index("/* Destination section", start)
region = source[start:end]
assert "type != XA_ATOM" in region
assert "format != 32" in region
assert "size != 0 && targets == NULL" in region
print("drag export-target conversion validates type, format, and payload")
