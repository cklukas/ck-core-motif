#!/usr/bin/env python3
"""Check fallback transfer callbacks receive the captured operation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("static void\nCallDoneProcs")
end = source.index("static TransferBlock\nAddTransferBlock", start)
body = source[start:end]

captured = body.index("op = tid->op;")
allocation = body.index("callbacks = (XmSelectionFinishedProc *)XtMalloc")
fallback = body.index("callback(wid, op, ts);")

assert captured < allocation < fallback
print("transfer callback fallback uses the captured operation")
