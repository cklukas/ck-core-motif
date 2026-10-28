#!/usr/bin/env python3
"""Check drag ICC reason-table conversions use bounded unsigned types."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragICC.c").read_text()
start = source.index("_XmReasonToMessageType(")
end = source.index("_XmICCCallbackToICCEvent(", start)
body = source[start:end]

assert "unsigned int i;" in body
assert "i < XtNumber(reasonTable)" in body
assert "return((unsigned int)messageTable[messageType]);" in body

print("Drag ICC reason-table conversions are explicit")
