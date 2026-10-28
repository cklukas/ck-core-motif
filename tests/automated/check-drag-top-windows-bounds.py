#!/usr/bin/env python3
"""Check TopWindowsReceived validates property data before rebuilding state."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragC.c").read_text()
start = source.index("TopWindowsReceived(", source.index("TopWindowsReceived(") + 1)
end = source.index("#endif /* MULTI_SCREEN_DONE */", start)
body = source[start:end]

assert "length == NULL || type == NULL ||" in body
assert "value != NULL && *length < (unsigned long)UINT_MAX" in body
assert "newInfos = (XmDragReceiverInfo)XtCalloc" in body
assert "if (newInfos == NULL)" in body
assert body.index("if (newInfos == NULL)") < body.index("XtFree((char *)startInfo)")

print("PASS DnD top-window property rebuild validates bounds and allocation")
