#!/usr/bin/env python3
"""Check transfer-id list unlinking stays under one process lock."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("static void\nFreeTransferID(")
end = source.index("static void\nCallDoneProcs", start)
body = source[start:end]

assert "if (tid == NULL)" in body
lock_start = body.index("_XmProcessLock();")
lock_end = body.index("_XmProcessUnlock();", lock_start)
locked = body[lock_start:lock_end]
for statement in ("global_tc = nid;", "pid -> next", "nid -> prev", "free_tc = tid;"):
    assert statement in locked, statement
assert body.count("_XmProcessLock();") == 1
assert body.count("_XmProcessUnlock();") == 1

print("transfer-id list unlinking is serialized")
