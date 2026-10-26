#!/usr/bin/env python3
"""Check that failed drag export conversion clears source context state."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmeDragSource(Widget w")
end = source.index("/* Destination section", start)
region = source[start:end]
for marker in ("if (type != XA_ATOM", "} else {\n    /* Free copied arguments */"):
    branch = region[region.index(marker):]
    assert "cc -> location_data = NULL;" in branch
    assert "cc -> client_data = NULL;" in branch
    assert "cc -> drag_context = NULL;" in branch
print("failed drag conversion paths clear source context state")
