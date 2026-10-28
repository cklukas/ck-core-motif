#!/usr/bin/env python3
"""Check drop-transfer termination rejects a missing ICC selection atom."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropTrans.c").read_text()
start = source.index("static void \nTerminateTransfer(")
end = source.index("static void\nFailTransferEntry", start)
terminate = source[start:end]
fail = source[end:source.index("static void \nProcessTransferEntry", end)]

assert "selection == NULL || *selection == None" in terminate
assert "if (selection == None)" in fail
assert "XtDestroyWidget((Widget)dt);" in fail

print("PASS drop-transfer termination rejects a missing selection atom")
