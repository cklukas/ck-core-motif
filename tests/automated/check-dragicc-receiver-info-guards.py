#!/usr/bin/env python3
from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/DragICC.c"
text = source.read_text()
start = text.index("Boolean \n_XmGetDragReceiverInfo")
body = text[start:text.index("/************************************************************************\n *\n *  _XmReadDSFromStream", start)]

assert "type == drag_hints_atom" in body
assert "format == 8" in body
assert "bytesafter == 0" in body
assert "heap_offset < sizeof(xmDragReceiverInfoStruct)" in body
assert "heap_offset > lengthRtn" in body
print("_XmGetDragReceiverInfo validates serialized property type and bounds")
