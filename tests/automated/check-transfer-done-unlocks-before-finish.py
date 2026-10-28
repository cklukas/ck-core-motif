#!/usr/bin/env python3
"""Check zero-request transfer finalization happens after app unlock."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("void\nXmTransferDone")
end = source.index("/************************************************************************/\n/* XmTransferSetParameters", start)
body = source[start:end]

assert "Boolean finish_after_unlock = False;" in body
assert "finish_after_unlock = True;" in body
unlock = body.index("_XmAppUnlock(app);")
finish = body.index("if (finish_after_unlock)")
assert unlock < finish
assert "FinishTransfer(tc -> widget, tc);" in body[finish:]
print("Transfer completion finalizes after app unlock")
