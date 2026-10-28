#!/usr/bin/env python3
"""Check TextField target discovery completes failed callbacks."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFSel.c").read_text()
start = source.index("\nHandleTargets(Widget")
start = source.index("  if (w == NULL", start)
end = source.index("\nstatic void", start + 1)
body = source[start:end]

assert "ds->transfer_id == NULL" in body
assert body.count("XmTransferDone(ds->transfer_id, XmTRANSFER_DONE_FAIL);") >= 3
assert "ds->value = NULL;" in body

print("TextField target discovery failures complete transfers")
