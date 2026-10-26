#!/usr/bin/env python3
"""Ensure clipboard-manager selection status has an unambiguous scope."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmeClipboardSource(")
end = source.index("/* XmeDragSource", start)
body = source[start:end]
manager = body[body.index("clipboard_owner ="):body.index("/* Use Motif clipboard", body.index("clipboard_owner ="))]

assert "int selection_status;" in manager
assert "int status;" not in manager
assert manager.count("selection_status = XtOwnSelection") == 2
assert "if (selection_status)" in manager

print("PASS clipboard-manager selection status has explicit scope")
