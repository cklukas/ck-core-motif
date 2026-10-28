#!/usr/bin/env python3
"""Check source-notification teardown handles destroyed objects safely."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropTrans.c").read_text()
start = source.index("static void \nSourceNotifiedCB(")
end = source.index("static void\nSetSelectionRequest", start)
body = source[start:end]

assert "if (dt == NULL)" in body
assert "if (dt->object.being_destroyed)" in body
assert body.count("XtFree((char *)value);") >= 2
assert body.index("dt->object.being_destroyed") < body.index("ForgetSelectionRequest(dt)")

print("PASS source-notification teardown handles destroyed transfers")
