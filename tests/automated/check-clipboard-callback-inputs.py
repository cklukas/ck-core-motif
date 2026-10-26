#!/usr/bin/env python3
"""Check that clipboard-manager callbacks validate inputs before X11 access."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("ClipboardCallback(Widget w")
end = source.index("\n/****************************************************************/\n/* XmeDragSource", start)
region = source[start:end]
guard = region.index("if (w == NULL || data_id == NULL || reason == NULL")
assert guard < region.index("XtDisplay(w)")
assert "DataIdDictionary == NULL" in region
print("clipboard-manager callback validates widget and request inputs")
