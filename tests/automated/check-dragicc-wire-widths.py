#!/usr/bin/env python3
"""Check Drag ICC callback fields explicitly enter 32-bit wire slots."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragICC.c").read_text()
start = source.index("_XmICCCallbackToICCEvent(")
end = source.index("_XmICCEventToICCCallback(", start)
body = source[start:end]

for field in ("timeStamp", "window", "iccHandle"):
    assert f"(CARD32)cb->{field}" in body

print("Drag ICC callback wire widths are explicit")
