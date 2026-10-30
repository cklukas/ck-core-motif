#!/usr/bin/env python3
"""Check dialog child lookup APIs validate their widget classes."""
from pathlib import Path

selection = (Path(__file__).parents[2] / "lib" / "Xm" / "SelectioB.c").read_text(encoding="utf-8")
start = selection.index("XmSelectionBoxGetChild(")
region = selection[start:selection.index("\n}\n", start)]
assert "if (sb == NULL || !XmIsSelectionBox(sb))" in region
assert region.index("!XmIsSelectionBox(sb)") < region.index("_XmWidgetToAppContext(sb)")

message = (Path(__file__).parents[2] / "lib" / "Xm" / "MessageB.c").read_text(encoding="utf-8")
start = message.index("XmMessageBoxGetChild(")
region = message[start:message.index("\n}\n", start)]
assert "if (widget == NULL || !XmIsMessageBox(widget))" in region
assert region.index("!XmIsMessageBox(widget)") < region.index("_XmWidgetToAppContext(widget)")

print("PASS dialog child lookup APIs validate widget classes before private access")
