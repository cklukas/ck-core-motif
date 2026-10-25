#!/usr/bin/env python3
"""Check that public clipboard copy lengths cannot truncate to int."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text(encoding="utf-8")
copy_start = source.index("int \nXmClipboardCopy(")
by_name_start = source.index("int \nXmClipboardCopyByName(", copy_start)
undo_start = source.index("int \nXmClipboardUndoCopy(", by_name_start)
copy = source[copy_start:by_name_start]
by_name = source[by_name_start:undo_start]

assert "length > INT_MAX" in copy
assert "length > INT_MAX" in by_name
print("PASS clipboard copy lengths are bounded before int conversion")
