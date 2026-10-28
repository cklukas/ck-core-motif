#!/usr/bin/env python3
"""Check drop-transfer processing rejects a missing transfer callback."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropTrans.c").read_text()
start = source.index("static void \nProcessTransferEntry(")
end = source.index("/* \n * This routine is called", start)
body = source[start:end]

assert "dtp->transfer_callback == NULL" in body
guard = body.index("dtp->transfer_callback == NULL")
assert body.index("FailTransferEntry(dt);", guard) < body.index("XtGetValues", guard)

print("PASS drop-transfer processing rejects a missing callback")
