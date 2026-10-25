#!/usr/bin/env python3
"""Check that serialized clipboard lock records are validated before use."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text(encoding="utf-8")
lock_start = source.index("static int \nClipboardLock(")
unlock_start = source.index("static int \nClipboardUnlock(", lock_start)
lock = source[lock_start:unlock_start]
unlock_end = source.index("static int \nClipboardSearchForWindow(", unlock_start)
unlock = source[unlock_start:unlock_end]

assert "status = ClipboardFindItem" in lock
assert "length < sizeof(ClipboardLockRec)" in lock
assert "status != ClipboardSuccess || length == 0" in lock
assert "status = ClipboardFindItem" in unlock
assert "length < sizeof(ClipboardLockRec)" in unlock
assert "lockptr->lockLevel <= 0" in unlock
print("PASS clipboard lock records are validated before field access")
