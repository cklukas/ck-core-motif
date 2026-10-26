#!/usr/bin/env python3
"""Check secondary-selection entry and callback input validation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmeSecondaryTransfer(")
end = source.index("/*****************************************************************/\n/* SecondaryDone", start)
entry = source[start:end]
assert "w == NULL || target == None" in entry
start = source.index("SecondaryDone(Widget wid")
end = source.index("Boolean\nXmeClipboardSource", start)
callback = source[start:end]
assert "wid == NULL" in callback
assert "type == NULL" in callback
assert "XtFree((char *)value);" in callback
print("secondary transfer entry and completion callbacks validate inputs")
