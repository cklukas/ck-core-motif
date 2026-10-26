#!/usr/bin/env python3
"""Ensure motion protocol handles receiver-info allocation failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragC.c").read_text()
start = source.index("static void \nDragMotionProto(")
body = source[start:source.index("static void \nProcessMotionBuffer", start)]
assert "GetDestinationInfo(dc, root, subWindow);" in body
assert "if (dc->drag.currReceiverInfo == NULL)" in body
print("PASS drag motion guards a missing receiver-info allocation")
