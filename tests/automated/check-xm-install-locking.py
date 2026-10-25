#!/usr/bin/env python3
"""Check XmInstallImage delegates locking to its synchronized helper."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()
start = source.index("\nXmInstallImage(")
start = source.index("Boolean", start - 80)
end = source.index("\n}\n", start) + 2
function = source[start:end]

assert "return _XmInstallImage(image, image_name, 0, 0);" in function
assert "_XmProcessLock" not in function
assert "_XmProcessUnlock" not in function
print("PASS XmInstallImage avoids nested process locking")
