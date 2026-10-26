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

mask_start = source.index("XmeGetMask(")
mask_end = source.index("\n}\n", mask_start) + 2
mask_function = source[mask_start:mask_end]
assert "_XmGetScaledPixmap" in mask_function
assert "XmGetPixmapByDepth" not in mask_function
print("PASS XmeGetMask avoids nested application locking")
