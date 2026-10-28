#!/usr/bin/env python3
"""Check drop-transfer processing fails closed after drag destruction."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropTrans.c").read_text()
process = source[source.index("static void \nProcessTransferEntry("):source.index("/* \n * This routine is called")]
timer = source[source.index("static void \nStartDropTimer("):source.index("static void\nDragContextDestroyCB")]

assert "if (dtp->dragContext == NULL)" in process
assert "dc->drag.currReceiverInfo == NULL" in process
assert "FailTransferEntry(dt);" in process
assert "if (dtp->dragContext == NULL)" in timer
assert timer.count("FailTransferEntry(dt);") >= 2

print("PASS drop-transfer processing fails closed after drag destruction")
