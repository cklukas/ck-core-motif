#!/usr/bin/env python3
"""Check DragUnder drop-site argument counts use Xt's Cardinal type."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragUnder.c").read_text()
start = source.index("CreateAnimationSaveData(")
end = source.index("static void\nFreeAnimationData", start)
body = source[start:end]

assert "Cardinal\t\t\tac;" in body
assert "int\t\t\t\tac;" not in body
assert "XmDropSiteRetrieve ((Widget) dc, al, ac);" in body

print("DragUnder argument counts match Xt")
