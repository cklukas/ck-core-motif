#!/usr/bin/env python3
"""Check that the persistent drag protocol window handles X creation failure."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "DragBS.c"
text = source.read_text()
start = text.index("\nCreateMotifWindow(")
end = text.index("\n}\n", start) + 3
block = text[start:end]

for needle in (
    "if (motifWindow == None)",
    "XUngrabServer(ndisplay);",
    "XCloseDisplay(ndisplay);",
    "return None;",
):
    if needle not in block:
        raise SystemExit(f"drag protocol window creation is missing {needle}")

if block.index("if (motifWindow == None)") > block.index("XMapWindow"):
    raise SystemExit("drag protocol window is mapped before creation is checked")

print("drag protocol window creation handles X failure")
