#!/usr/bin/env python3
"""Check text drop transfer record allocation is handled."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextSel.c").read_text()
allocation = "XtMalloc(sizeof(_XmTextDropTransferRec));"
start = source.index(allocation)
guard = source.index("if (transfer_rec == NULL)", start)
assert start < guard
block = source[guard:source.index("transfer_rec->widget", guard)]
assert "XmTransferDone(tid, XmTRANSFER_DONE_FAIL);" in block
assert "return;" in block
print("PASS text drop transfer allocation failure is handled")
