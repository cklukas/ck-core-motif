#!/usr/bin/env python3
"""Ensure Compound String drawing does not narrow huge segments for Xlib."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text(
    encoding="utf-8"
)
start = source.index("_XmStringDrawSegment(")
body = source[start : source.index("\n}\n\f\n/****************************************************************", start)]
read = body.index("seg_len = _XmEntryByteCountGet")
guard = body.index("if (seg_len > (unsigned int)INT_MAX)", read)
draw = body.index("if (seg_len  > 0)", guard)
assert read < guard < draw
assert "return;" in body[guard:draw]
for call in ("_XmXftDrawString", "XDrawImageString16", "XDrawImageString (",
             "XDrawString16", "XDrawString(d", "XmbDrawString",
             "Xutf8DrawString"):
    assert body.index(call) > guard
print("PASS Compound String drawing rejects lengths outside Xlib's int contract")
