#!/usr/bin/env python3
"""Check clipboard-source failure exits clean manager-backed ownership."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmeClipboardSource(Widget w, XtEnum op, Time time)")
end = source.index("/********************************************************", start)
body = source[start:end]

assert "AbortClipboardSource(" in body
assert body.count("AbortClipboardSource(w, display, atoms[XmACLIPBOARD]") >= 6
assert "manager_owned)" in source

print("PASS clipboard-source failure exits clean manager ownership")
