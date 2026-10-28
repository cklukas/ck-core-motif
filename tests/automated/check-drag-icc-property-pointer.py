#!/usr/bin/env python3
"""Check ICC drag properties validate returned data pointers."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragICC.c").read_text()
start = source.index("_XmWriteInitiatorInfo(")
end = source.index("_XmGetDragReceiverInfo(", start)
initiator = source[start:end]
receiver = source[end:]

assert "info != NULL &&" in initiator
assert "iccInfo != NULL &&" in receiver
assert "XFree( (void *)info)" in initiator
assert "XFree((void *)iccInfo)" in receiver

print("ICC drag property readers validate returned data pointers")
