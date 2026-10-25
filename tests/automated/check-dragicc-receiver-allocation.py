#!/usr/bin/env python3
from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/DragICC.c"
text = source.read_text()
start = text.index("Boolean \n_XmGetDragReceiverInfo")
body = text[start:text.index("/************************************************************************\n *\n *  _XmReadDSFromStream", start)]

assert "dsmInfo = XtNew(XmReceiverDSTreeStruct)" in body
assert "if (dsmInfo == NULL)" in body
assert "XFree((void *)iccInfo)" in body
print("_XmGetDragReceiverInfo handles drop-site state allocation failure")
