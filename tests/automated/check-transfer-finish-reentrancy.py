#!/usr/bin/env python3
"""Check transfer completion is guarded against reentrancy."""

from pathlib import Path


header = (Path(__file__).parents[2] / "lib" / "Xm" / "TransferI.h").read_text()
source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()

assert "TC_FINISHED = 64" in header
start = source.index("static void\nFinishTransfer")
end = source.index("\n/****************************************************************", start)
body = source[start:end]
assert "if (tc->flags & TC_FINISHED)" in body
assert "tc->flags |= TC_FINISHED;" in body
assert "_XmProcessUnlock();" in body

print("Transfer completion is reentrancy-safe")
