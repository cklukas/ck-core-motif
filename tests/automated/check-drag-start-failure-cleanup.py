#!/usr/bin/env python3
"""Check that a failed drag start clears its conversion context payload."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmeDragSource(Widget w")
end = source.index("/* Destination section", start)
region = source[start:end]
failure = region[region.index("dragContext = XmDragStart"):]
assert "if (dragContext == NULL)" in failure
assert "cc -> location_data = NULL;" in failure
assert "cc -> client_data = NULL;" in failure
print("failed drag start clears conversion context payload")
