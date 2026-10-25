#!/usr/bin/env python3
"""Check text-field drop coordinates outlive the destination callback."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFSel.c").read_text()
start = source.index("TextFieldDestinationCallback(Widget w")
end = source.index("\n}\n", start) + 2
function = source[start:end]

assert "XPoint *drop_point;" in function
assert "XtMalloc(sizeof(*drop_point))" in function
assert "XmeTransferAddDoneProc(ds->transfer_id, FreeLocationData);" in function
assert "DropPoint" not in function
print("PASS text-field drop location lifetime")
