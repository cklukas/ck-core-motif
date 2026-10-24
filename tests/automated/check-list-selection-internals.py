#!/usr/bin/env python3
"""Check internal List selection-builder count bounds."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "List.c"
text = source.read_text(encoding="utf-8")
for needle in (
    "(size_t)j > SIZE_MAX / sizeof(XmString)",
    "(size_t)nsel > SIZE_MAX / sizeof(int)",
    "if (nsel <= 0 ||",
):
    if needle not in text:
        raise SystemExit(f"List selection-builder guard missing: {needle}")

print("List internal selection sizing is bounded")
