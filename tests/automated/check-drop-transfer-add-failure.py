#!/usr/bin/env python3
"""Check checked drop-transfer additions roll back failed requests."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("void \nXmTransferValue")
end = source.index("/************************************************************************/\n/* XmTransferStartRequest", start)
body = source[start:end]

assert "_XmDropTransferAddChecked(tc -> drop_context, transfers, 1)" in body
failure = body.index("_XmDropTransferAddChecked")
branch = body[failure:body.index("\n\t}\n\n      tc -> outstanding++;", failure)]
assert "RemoveTransferBlock(tc, tb);" in branch
assert "tc -> status = XmTRANSFER_DONE_FAIL;" in branch
assert "FinishTransfer(tc -> widget, tc);" in branch

drop_source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropTrans.c").read_text()
private = drop_source[drop_source.index("Boolean\n_XmDropTransferAddChecked"):]
assert "old_count" in private
assert "old_count != UINT_MAX" in private
assert "num_drop_transfer_lists == old_count + 1" in private
assert "!XtIsSubclass(widget, xmDropTransferObjectClass)" in private

print("PASS failed drop-transfer additions roll back requests")
