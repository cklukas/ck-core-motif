#!/usr/bin/env python3
"""Check TextField raw transfer conversion validates its format."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFSel.c").read_text()
start = source.index("static void\nDoStuff")
start = source.index("      if (ds->format", start)
body = source[start:start + 420]

assert "ds->format != 8" in body
assert "ds->length > (unsigned long)INT_MAX" in body
assert "XmTransferDone(ds->transfer_id, XmTRANSFER_DONE_FAIL);" in body

print("TextField raw transfers reject non-byte formats")
