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
        "sizeof(RGMCallbackItem)",
        "item_count >\n\t (USHRT_MAX - sizeof(RGMCallbackDesc))",
    ):
        if needle not in region:
            raise SystemExit(f"{function} guard missing: {needle}")
    if "(size_t)nitems == SIZE_MAX" not in region and \
       "(size_t)nitems > SIZE_MAX" not in region:
        raise SystemExit(f"{function} nitems conversion guard missing")
    if "descsiz = sizeof(RGMCallbackDesc) + item_count*sizeof(RGMCallbackItem)" not in region:
        raise SystemExit(f"{function} descriptor sizing missing")

print("Mrm callback descriptor sizing is bounded")
