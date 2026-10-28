#!/usr/bin/env python3
"""Check XmDropTransferAdd rejects non-drop-transfer widgets."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropTrans.c").read_text()
start = source.index("Boolean\n_XmDropTransferAddChecked(")
end = source.index("void\nXmDropTransferAdd(", start)
body = source[start:end]

assert "!XtIsSubclass(widget, xmDropTransferObjectClass)" in body
assert "num_transfers == 0 || transfers == NULL" in body
assert body.index("!XtIsSubclass(widget, xmDropTransferObjectClass)") < body.index("_XmWidgetToAppContext(widget)")

print("PASS drop-transfer append rejects non-drop-transfer widgets")
