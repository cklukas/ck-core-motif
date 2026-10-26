#!/usr/bin/env python3
"""Ensure DnD receiver properties reject unrepresentable buffer sizes."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragICC.c").read_text()
start = source.rindex("_XmSetDragReceiverInfo(")
body = source[start:source.index("/************************************************************************", start + 10)]
guard = body.index("if (numDropSites > UINT16_MAX")
before_property = body[guard:body.index("/* write the buffer to the property */", guard)]
assert "propBuf->data.size > (size_t)INT_MAX" in before_property
assert "propBuf->heap.size > (size_t)INT_MAX" in before_property
assert "return;" in before_property
assert "(CARD32)propBuf->data.size" in before_property
assert "(int)propBuf->data.size" in body
assert "(int)propBuf->heap.size" in body
print("PASS DnD property sizes are checked before publication")
