#!/usr/bin/env python3
"""Check transfer done-proc reclamation is serialized with registration."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("FreeTransferID(XtPointer id)")
end = source.index("static void\nCallDoneProcs", start)
body = source[start:end]
lock = body.index("_XmProcessLock();")
free = body.index("XtFree((char*) tid -> doneProcs)")
assert lock < free
assert "tid -> doneProcs = NULL" in body
assert "tid -> numDoneProcs = 0" in body

print("Transfer done-proc reclamation is serialized")
