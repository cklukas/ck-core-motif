#!/usr/bin/env python3
"""Check that legacy XmString constructors stop after concat failure."""

from pathlib import Path


source = Path("lib/Xm/XmStringObso.c").read_text()

l2r_start = source.index("XmStringLtoRCreate(")
l2r_end = source.index("\n}\n", l2r_start)
l2r = source[l2r_start:l2r_end]
assert l2r.count("if (string == NULL)") >= 2

segment_start = source.index("XmStringSegmentCreate(")
segment_end = source.index("\n}\n", segment_start)
segment = source[segment_start:segment_end]
assert "if (result == NULL)" in segment
assert "return (NULL);" in segment

print("legacy XmString constructors propagate concat failures")
