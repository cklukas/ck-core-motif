#!/usr/bin/env python3
"""Check drop-transfer processing failures terminate with failure status."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropTrans.c").read_text()
start = source.index("static void \nProcessTransferEntry(")
end = source.index("/* \n * This routine is called", start)
body = source[start:end]

assert "FailTransferEntry(dt);" in body
assert body.count("FailTransferEntry(dt);") >= 4
helper = source[source.index("static void\nFailTransferEntry"):start]
assert "dtp->transfer_status = XmTRANSFER_FAILURE;" in helper
assert "TerminateTransfer(dt, &selection);" in helper
assert "XtFree((char *)dtp->cur_targets);" in helper

print("PASS drop-transfer processing failures terminate cleanly")
