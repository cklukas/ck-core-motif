#!/usr/bin/env python3
"""Check transfer-block allocation failure completes and frees the context."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("void \nXmTransferValue(")
end = source.index("/************************************************************************/\n/* XmTransferStartRequest", start)
region = source[start:end]

failure = region.index("tb = AddTransferBlock(tc);")
branch = region[failure:region.index("\n  }", failure) + 4]
assert "if (tb == NULL)" in branch
assert "tc -> status = XmTRANSFER_DONE_FAIL;" in branch
assert "finish_after_unlock" in branch
assert "FinishTransfer(tc -> widget, tc);" in branch

print("PASS transfer-block allocation failure completes transfer teardown")
