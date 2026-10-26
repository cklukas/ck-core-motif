#!/usr/bin/env python3
"""Ensure transfer-entry work arrays are bounded and failure-safe."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropTrans.c").read_text()
start = source.index("static void \nProcessTransferEntry(")
body = source[start:]
assert body.count("UINT_MAX / sizeof") >= 2
assert "tl->num_transfers == 0" in body
assert body.count("== NULL") >= 2
assert "XtFree((char *)dtp->cur_targets)" in body
print("PASS drop-transfer entry arrays handle bounds and allocation failure")
