#!/usr/bin/env python3
"""Check malformed drop-transfer callbacks release selection values."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropTrans.c").read_text()
start = source.index("DropTransferSelectionCB(")
end = source.index("/*ARGSUSED*/\nstatic void \nStartDropTimer", start)
body = source[start:end]

assert body.count("XtFree((char *)value);") >= 3
assert body.count("FailTransferEntry(dt);") >= 3
assert "XtFree((char *)value);\n\t    FailTransferEntry(dt);" in body
assert "if (*length == 0 && value != NULL)" in body
assert "if ((*format != 8 && *format != 16 && *format != 32)" in body

print("PASS rejected drop-transfer values are released")
