#!/usr/bin/env python3
"""Check Text target discovery completes failed callbacks."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextSel.c").read_text()
start = source.index("HandleTargets")
start = source.index("  if (w == NULL", start)
end = source.index("\n/* Pastes the primary selection", start)
body = source[start:end]

assert "ds->transfer_id != NULL" in body
assert body.count("XmTransferDone(ds->transfer_id, XmTRANSFER_DONE_FAIL);") >= 3
assert "ds->value = NULL;" in body

print("Text target discovery failures complete transfers")
