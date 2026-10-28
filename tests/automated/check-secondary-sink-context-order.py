#!/usr/bin/env python3
"""Check secondary-sink context replacement follows ownership success."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmeSecondarySink(")
end = source.index("/**************************************************************/", start)
body = source[start:end]

own = body.index("status = XtOwnSelection")
clear = body.index("ClearContextBlock", own)
assert body.find("ClearContextBlock", 0, own) == -1
assert body.index("if (status)", own) < clear

print("PASS secondary sink preserves context until ownership succeeds")
