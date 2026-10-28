#!/usr/bin/env python3
"""Check drop-transfer callbacks reject a null selection atom."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/DropTrans.c").read_text()
start = source.index("static void \nDropTransferSelectionCB")
end = source.index("/*ARGSUSED*/\nstatic void \nStartDropTimer", start)
body = source[start:end]

assert "selection == NULL || *selection == None" in body
assert "FailTransferEntry(dt);" in body

print("PASS drop-transfer callbacks reject null selection atoms")
