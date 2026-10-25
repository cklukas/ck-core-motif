#!/usr/bin/env python3
"""Check Xm scratch-pixmap allocation failure handling."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Screen.c").read_text(encoding="utf-8")
start = source.index("_XmAllocScratchPixmap(")
end = source.index("/************************************************************************\n *\n *  _XmFreeScratchPixmap", start)
body = source[start:end]
assert "if (xmScreen == NULL)" in body
assert "if (scratchTable == NULL || inUseTable == NULL)" in body
assert "if (returned_key == NULL)" in body
assert "if (scratchPixmap == None)" in body
assert "XtFree((char *)returned_key);" in body
assert body.index("if (scratchPixmap == None)") < body.index("_XmAddHashEntry(inUseTable")
print("Xm scratch-pixmap allocation failures roll back safely")
