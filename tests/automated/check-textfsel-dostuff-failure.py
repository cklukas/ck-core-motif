#!/usr/bin/env python3
"""Check malformed TextField transfer callbacks complete with failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextFSel.c").read_text()
start = source.index("static void\nDoStuff")
end = source.index("\n  tf = (XmTextFieldWidget) w;", start)
body = source[start:end]

assert "if (w == NULL || ds == NULL || closure == NULL) {" in body
assert "ds->transfer_id != NULL" in body
assert "XmTRANSFER_DONE_FAIL" in body

print("Malformed TextField transfer callbacks fail cleanly")
