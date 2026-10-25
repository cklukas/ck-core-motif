#!/usr/bin/env python3
"""Check that clipboard item lookup validates outputs and item atoms."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text(encoding="utf-8")
start = source.index("static int \nClipboardFindItem(")
end = source.index("static int \nGetWindowProperty(", start)
region = source[start:end]

assert "outpointer == NULL || outlength == NULL" in region
assert "*outpointer = NULL;" in region
assert "*outlength = 0;" in region
assert "itematom == None" in region
assert "return ClipboardFail;" in region
print("PASS clipboard item lookup outputs and atoms are guarded")
