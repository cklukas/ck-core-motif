#!/usr/bin/env python3
from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/DragICC.c"
text = source.read_text()
start = text.index("void \n_XmReadInitiatorInfo")
body = text[start:text.index("/************************************************************************\n *\n *  _XmGetDragReceiverInfo", start)]

assert "type == initiatorAtom" in body
assert "format == 8" in body
assert "bytesafter == 0" in body
assert "lengthRtn >= sizeof(xmDragInitiatorInfoStruct)" in body
print("_XmReadInitiatorInfo validates serialized property type and length")
