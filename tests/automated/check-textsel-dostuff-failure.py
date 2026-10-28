#!/usr/bin/env python3
"""Check malformed Text transfer callbacks complete with failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextSel.c").read_text()
start = source.index("static void\nDoStuff")
end = source.index("\n  tw = (XmTextWidget) w;", start)
body = source[start:end]

assert "if (w == NULL || ds == NULL || closure == NULL) {" in body
assert "ds->transfer_id != NULL" in body
assert "XmTRANSFER_DONE_FAIL" in body

print("Malformed Text transfer callbacks fail cleanly")
