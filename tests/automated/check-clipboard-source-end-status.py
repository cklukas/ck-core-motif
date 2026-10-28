#!/usr/bin/env python3
"""Check clipboard-source end failures cancel unfinished copies."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
helper_start = source.index("EndClipboardSource(Display *display")
helper_end = source.index("\n}\n\nBoolean", helper_start) + 2
helper = source[helper_start:helper_end]
start = source.index("XmeClipboardSource(Widget w, XtEnum op, Time time)")
end = source.index("/********************************************************", start)
body = source[start:end]

assert "XmClipboardEndCopy" in helper
assert "if (status != XmClipboardSuccess)" in helper
assert "XmClipboardCancelCopy" in helper
assert "EndClipboardSource(display, XtWindow(w), itemid) != XmClipboardSuccess" in body

print("PASS clipboard-source end failures cancel unfinished copies")
