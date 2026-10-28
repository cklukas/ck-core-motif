#!/usr/bin/env python3
"""Check the delayed drop-transfer timer validates its client data."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropTrans.c").read_text()
start = source.index("static void \nStartDropTimer")
end = source.index("static Widget \nStartDropTransfer", start)
body = source[start:end]

assert "if (dt == NULL)" in body
assert body.index("if (dt == NULL)") < body.index("dtp =")

print("PASS drop-transfer timer validates client data")
