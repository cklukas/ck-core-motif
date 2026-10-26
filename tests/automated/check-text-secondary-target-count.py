#!/usr/bin/env python3
"""Check secondary Text target counts are bounded before iteration."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextSel.c").read_text()
start = source.index("HandleInsertTargets(")
end = source.index("\nstatic void\nHandleDrop", start)
body = source[start:end]
assert "*length > (unsigned long)INT_MAX" in body
assert "XmTransferDone(tid, XmTRANSFER_DONE_FAIL);" in body
assert "for (i = 0; i < *length; i++, atom_ptr++)" in body

print("Text secondary target counts are bounded")
