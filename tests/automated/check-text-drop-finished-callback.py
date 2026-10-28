#!/usr/bin/env python3
"""Check text drop cleanup uses XmSelectionFinishedProc directly."""
from pathlib import Path

root = Path(__file__).parents[2]
for name in ("TextSel.c", "TextFSel.c"):
    source = (root / "lib" / "Xm" / name).read_text(encoding="utf-8")
    assert "XmSelectionFinishedProc) DropDestroyCB" not in source
    assert "XmeTransferAddDoneProc(tid, DropDestroyCB);" in source
    start = source.rindex("DropDestroyCB(Widget")
    body = source[start : source.index("\n}", start) + 2]
    assert "XtEnum" in body
    assert "XmTransferDoneCallbackStruct *callData" in body
    assert "(void)unused;" in body
print("PASS text drop cleanup uses the exact finished-callback ABI")
