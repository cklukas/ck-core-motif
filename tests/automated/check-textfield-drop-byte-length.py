#!/usr/bin/env python3
"""Check TextField drop conversion bounds the single-byte length cast."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFSel.c").read_text()
start = source.index("static void \nDropTransferProc")
end = source.index("\n#define CR1228", start)
body = source[start:end]

guard = "if (total_length > (unsigned long)INT_MAX)"
assert guard in body
assert "(int)total_length" in body
assert "XmTransferDone(ds->transfer_id, XmTRANSFER_DONE_FAIL);" in body

print("TextField drop single-byte lengths are bounded before casting")
