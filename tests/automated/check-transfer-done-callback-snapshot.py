#!/usr/bin/env python3
"""Check transfer completion callbacks are snapshotted before invocation."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("static void\nCallDoneProcs")
end = source.index("\nstatic TransferBlock", start)
body = source[start:end]

assert "_XmProcessLock();" in body
assert "memcpy(callbacks, tid->doneProcs" in body
assert "_XmProcessUnlock();" in body
assert "callbacks[i](wid, op, ts);" in body
assert "XtFree((char *)callbacks);" in body

print("Transfer completion callbacks are snapshotted safely")
