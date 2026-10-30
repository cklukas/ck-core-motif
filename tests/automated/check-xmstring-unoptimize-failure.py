#!/usr/bin/env python3
"""Ensure XmString concat handles failed optimized-entry conversion safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
helper_start = source.index("static _XmStringEntry\nUnoptimize")
helper_end = source.index("XmString \nXmStringConcat(", helper_start)
helper = source[helper_start:helper_end]
assert "if (new_entry != NULL && free)" in helper
assert "if (converted == NULL)" in helper
assert "_XmEntrySegment(entry)[j] = converted;" in helper

concat_start = source.index("XmString \nXmStringConcatAndFree")
concat = source[concat_start:]
assert concat.count("if (b_seg == NULL)") >= 1
assert concat.count("if (b_line == NULL)") >= 1
assert "if (converted == NULL)" in concat
for marker in ("if (converted == NULL)", "if (b_seg == NULL)", "if (b_line == NULL)"):
    pos = concat.index(marker)
    assert "XmStringFree(a);" in concat[pos:pos + 260]
    assert "return NULL;" in concat[pos:pos + 260]
print("PASS XmString concat handles Unoptimize failure without NULL dereference")
