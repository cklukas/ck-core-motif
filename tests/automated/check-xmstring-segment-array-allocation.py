#!/usr/bin/env python3
"""Check XmString concat segment-array sizing and allocation failures."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("if (b_sc - merged > 0 && _XmStrImplicitLine(a_str))")
body = source[start:source.index("for (i = 0; i < (b_sc - merged); i++)", start)]
assert "new_segment_count = (size_t)a_sc + (size_t)b_sc" in body
assert "SIZE_MAX / sizeof(*new_segments)" in body
assert "UINT_MAX / sizeof(*new_segments)" in body
assert "if (new_segments == NULL)" in body
assert "if (a_line == NULL)" in body
assert "_XmEntrySegment(a_line) == NULL" in body
print("PASS XmString segment-array allocation bounds and failures")
