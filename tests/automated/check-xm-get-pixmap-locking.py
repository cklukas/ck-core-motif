#!/usr/bin/env python3
"""Check XmGetPixmap does not recursively acquire the application lock."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("XmGetPixmap(\n")
end = source.index("\n}\n", start) + 2
function = source[start:end]

assert "_XmProcessLock();" in function
assert "_XmProcessUnlock();" in function
assert "_XmGetScaledPixmap" in function
assert "XmGetPixmapByDepth" not in function
print("PASS XmGetPixmap avoids nested application locking")
