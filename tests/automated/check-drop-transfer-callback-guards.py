#!/usr/bin/env python3
"""Check drop-transfer selection callbacks validate callback state."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropTrans.c").read_text()
timer_start = source.index("static void \nStartDropTimer")
timer_end = source.index("static Widget \nStartDropTransfer", timer_start)
timer = source[timer_start:timer_end]
assert "if (dt == NULL)" in timer
assert timer.index("if (dt == NULL)") < timer.index("dtp =")
start = source.index("static void \nDropTransferSelectionCB(")
end = source.index("/*ARGSUSED*/\nstatic void \nStartDropTimer", start)
body = source[start:end]

assert "if (client_data == NULL)" in body
assert "selection == NULL || *selection == None || type == NULL || length == NULL" in body
assert "dtp->drop_transfer_lists == NULL" in body
assert "dtp->cur_drop_transfer_list >= dtp->num_drop_transfer_lists" in body
assert "tl->transfer_list == NULL || dtp->cur_xfer >= tl->num_transfers" in body
assert body.count("FailTransferEntry(dt);") >= 2

print("PASS drop-transfer selection callbacks validate state")
