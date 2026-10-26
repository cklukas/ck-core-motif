#!/usr/bin/env python3
"""Ensure ICC stream writing validates region inputs before dereferencing."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragICC.c").read_text()
start = source.index("void \n_XmWriteDSToStream(")
body = source[start:source.index("/************************************************************************\n *\n *  _XmFreeDragReceiverInfo", start)]
assert "region == NULL" in body
assert "region->rects == NULL" in body
assert "region->numRects > UINT32_MAX" in body
assert "if (region == NULL" in body
print("PASS ICC stream writing validates region inputs")
