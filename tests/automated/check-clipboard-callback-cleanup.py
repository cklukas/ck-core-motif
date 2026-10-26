#!/usr/bin/env python3
"""Check that malformed clipboard data still reaches request completion."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("ClipboardCallback(Widget w")
end = source.index("/* XmeDragSource", start)
region = source[start:end]
failure = region[region.index("if (unit == 0"):]
assert "XmClipboardCopyByName(display, XtWindow(w), *data_id," in failure
assert "NULL, 0, 0L);" in failure
assert failure.index("NULL, 0, 0L);") < failure.index("if (req -> outstanding == 0)")
print("malformed clipboard data follows the normal request cleanup path")
