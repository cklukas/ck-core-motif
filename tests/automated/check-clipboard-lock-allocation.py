#!/usr/bin/env python3
"""Check that clipboard lock allocation failure is handled safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text(encoding="utf-8")
start = source.index("static int \nClipboardLock(")
end = source.index("static int \nClipboardUnlock(", start)
region = source[start:end]

allocation = "lockptr = (ClipboardLockPtr)XtMalloc(sizeof(ClipboardLockRec));"
assert allocation in region
guard = region.index("if (lockptr == NULL)")
assert guard > region.index(allocation)
assert "_XmAppUnlock(app);" in region[guard:guard + 120]
assert "return (ClipboardFail);" in region[guard:guard + 120]
print("PASS clipboard lock allocation failure is guarded")
