#!/usr/bin/env python3
"""Check drop-transfer start validates arguments and class lookup."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropTrans.c").read_text()
start = source.index("Widget \nXmDropTransferStart(")
end = source.index("static void \nAddDropTransfer(", start)
body = source[start:end]

assert "if (refWidget == NULL || !XtIsSubclass(refWidget, xmDragContextClass) ||" in body
assert "(argCount != 0 && args == NULL)" in body
assert "if (ddo == NULL)" in body
assert "wc == NULL || wc->dropTransfer_class.start_drop_transfer == NULL" in body

print("PASS drop-transfer start validates arguments and class lookup")
