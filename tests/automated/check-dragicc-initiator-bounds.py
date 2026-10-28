#!/usr/bin/env python3
"""Check Drag ICC initiator metadata fits its wire fields."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragICC.c").read_text()
start = source.index("_XmWriteInitiatorInfo(")
end = source.index("_XmReadInitiatorInfo(", start)
body = source[start:end]

assert "Cardinal\t\ti = 0;" in body
assert "targetsIndex > UINT16_MAX" in body
assert "infoRec.targets_index = (CARD16)targetsIndex;" in body
assert "infoRec.icc_handle = (CARD32)iccHandle;" in body

read_start = source.index("_XmReadInitiatorInfo(")
read_end = source.index("_XmGetDragReceiverInfo(", read_start)
read_body = source[read_start:read_end]
assert "Cardinal\t\t\ti;" in read_body

print("Drag ICC initiator metadata is wire-bounded")
