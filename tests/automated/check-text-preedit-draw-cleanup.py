#!/usr/bin/env python3
"""Check XmText PreeditDraw temporary-buffer cleanup on aborts."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Text.c").read_text()
start = source.index("static void\nPreeditDraw(")
end = source.index("\n\n/*\n * This is the function set to XNPreeditCaretCallback", start)
body = source[start:end]

assert "if (end_preedit) {" in body
assert "XtFree(mb);\n      ResetUnder(tw);\n      return;" in body
assert "!= EditDone) {" in body
assert "XBell(XtDisplay(tw), 0);\n      (*tw->text.output->DrawInsertionPoint" in body
assert body.count("ResetUnder(tw);") >= 3

print("XmText PreeditDraw abort paths clean up temporary buffers")
