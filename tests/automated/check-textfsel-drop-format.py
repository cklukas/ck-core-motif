#!/usr/bin/env python3
"""Check file-selection drops validate the external selection format."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFSel.c").read_text()
start = source.index("  if (!(ds->value)")
end = source.index("\n  insertPosLeft", start)
body = source[start:end]

assert "ds->format != 8" in body
assert "ds->length > (unsigned long)INT_MAX" in body
assert "XmTransferDone(ds->transfer_id, XmTRANSFER_DONE_FAIL);" in body

print("File-selection drops reject non-byte formats")
