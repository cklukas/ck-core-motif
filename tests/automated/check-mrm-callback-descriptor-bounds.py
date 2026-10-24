#!/usr/bin/env python3
"""Check callback descriptor constructors before 16-bit record sizing."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c"
text = source.read_text(encoding="utf-8")
for function in ("UrmCWRSetArgCallback", "UrmCWRSetCreationCallback"):
    start = text.index(function + " (")
    end = text.index("\n}\n", start)
    region = text[start:end]
    for needle in (
        "nitems > RGMListSizeMax",
        "sizeof(RGMCallbackDesc) > USHRT_MAX",
        "(size_t)nitems >",
        "sizeof(RGMCallbackItem)",
        "size_t\t\tdescsiz",
    ):
        if needle not in region:
            raise SystemExit(f"{function} guard missing: {needle}")

print("Mrm callback descriptor sizing is bounded")
