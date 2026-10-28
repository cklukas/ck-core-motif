#!/usr/bin/env python3
"""Check drop-transfer construction handles widget allocation failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropTrans.c").read_text()
start = source.index("static Widget \nStartDropTransfer(")
end = source.index("Widget \nXmDropTransferStart(", start)
body = source[start:end]

assert "dt = (XmDropTransferObject) XtCreateWidget" in body
guard = body.index("if (dt == NULL)")
assert guard < body.index("dt->dropTransfer.dragContext")
assert "return NULL;" in body[guard:]

print("PASS drop-transfer construction handles allocation failure")
