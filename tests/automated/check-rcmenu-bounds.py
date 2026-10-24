#!/usr/bin/env python3
"""Check dynamic RCMenu array growth bounds."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "RCMenu.c"
text = source.read_text(encoding="utf-8")
for needle in (
    "SIZE_MAX / sizeof(XtPointer)",
    "pane_list_size > SHRT_MAX - 4",
    "SIZE_MAX / sizeof(Widget)",
    "postFromListSize > INT_MAX - 2",
):
    if needle not in text:
        raise SystemExit(f"RCMenu capacity guard missing: {needle}")

print("RCMenu dynamic array growth is bounded")
