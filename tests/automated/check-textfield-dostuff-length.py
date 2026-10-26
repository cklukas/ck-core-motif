#!/usr/bin/env python3
"""Check TextField DoStuff bounds external conversion lengths."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFSel.c").read_text()
start = source.index("static void\nDoStuff(")
end = source.index("\nstatic void \nHandleTargets", start)
body = source[start:end]
assert "ds->length > (unsigned long)INT_MAX" in body
assert "ds->length > (unsigned long)UINT_MAX / sizeof(wchar_t) - 1" in body
assert "if (temp == NULL)" in body
assert "if (wc_value == NULL)" in body
assert "XmTransferDone(ds->transfer_id, XmTRANSFER_DONE_FAIL);" in body

print("TextField DoStuff conversion lengths are bounded")
