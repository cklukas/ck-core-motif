#!/usr/bin/env python3
"""Check drop-transfer request state allocation failure handling."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DataF.c").read_text(
    encoding="utf-8"
)
start = source.rindex("transfer_rec = (_XmTextDropTransferRec *)")
end = source.index("transfer_rec->widget", start)
context = source[start:end]
required = (
    "if (transfer_rec == NULL)",
    "XmNtransferStatus, XmTRANSFER_FAILURE",
    "XmNnumDropTransfers, 0",
    "XmDropTransferStart(drag_cont, args, n);",
    "return;",
)
missing = [item for item in required if item not in context]
if missing:
    raise SystemExit("drop-transfer allocation guard missing: " + ", ".join(missing))

print("DataField drop-transfer checks passed")
