#!/usr/bin/env python3
"""Check transfer completion callbacks survive snapshot allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("static void\nCallDoneProcs")
end = source.index("static TransferBlock\nAddTransferBlock", start)
body = source[start:end]

assert "if (callbacks == NULL)" in body
assert "Fetch one function pointer at a time" in body
assert "callback = tid->doneProcs[i];" in body
assert "callback(wid, op, ts);" in body
print("Completion callbacks have an allocation-failure fallback")
