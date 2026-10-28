#!/usr/bin/env python3
"""Check standard WM property conversion rejects failed/partial reads."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("} else if (atoms[XmACLASS] == cs -> target)")
end = source.index("} else if (atoms[XmACLIENT_WINDOW] == cs -> target)", start)
body = source[start:end]

assert body.count("result = XGetWindowProperty") == 2
assert body.count("result == Success && bytesAfter == 0") == 2
assert body.count("XFree((char *)") >= 2

print("Standard WM property conversion rejects failed and partial reads")
