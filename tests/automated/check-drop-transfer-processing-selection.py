#!/usr/bin/env python3
"""Check drop-transfer processing rejects a missing ICC selection atom."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/DropTrans.c").read_text()
start = source.index("XtSetArg(args[i], XmNiccHandle")
end = source.index("requestor = dc->drag.currReceiverInfo->shell", start)
body = source[start:end]

assert "if (real_selection_atom == None)" in body
assert "FailTransferEntry(dt);" in body
assert "return;" in body

print("PASS drop-transfer processing validates ICC selection")
