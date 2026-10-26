#!/usr/bin/env python3
"""Ensure Atom private IDs are widened explicitly for CutPaste APIs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmClipboardCopy(display, XtWindow(w), itemid, name,\n\t\t\tNULL, 0,")
end = source.index("\n\t_XmProcessUnlock();", start)
body = source[start:end]

assert "NULL, 0, (long)targets[i], &data_id);" in body
assert "NULL, 0, targets[i], &data_id);" not in body

print("PASS clipboard Atom private ID widening is explicit")
