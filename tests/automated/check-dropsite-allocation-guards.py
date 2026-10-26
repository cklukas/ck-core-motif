#!/usr/bin/env python3
"""Check that drop-site records handle region and record allocation failure."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "DropSMgr.c"
text = source.read_text()

for function, needles in {
    "CreateShellDSInfo": (
        "if (region == NULL)",
        "if (info == NULL)",
        "_XmRegionDestroy(region);",
    ),
    "CreateClipperDSInfo": (
        "if (region == NULL)",
        "if (info == NULL)",
        "_XmRegionDestroy(region);",
    ),
}.items():
    start = text.index(f"\n{function}(" )
    end = text.index("\n}\n", start) + 3
    block = text[start:end]
    for needle in needles:
        if needle not in block:
            raise SystemExit(f"{function} is missing {needle}")

start = text.index("\nInsertInfo(")
end = text.index("\n}\n", start) + 3
block = text[start:end]
if "if (parentInfo == NULL)" not in block:
    raise SystemExit("InsertInfo does not handle shell-record allocation failure")

start = text.index("\nCreateInfo(")
end = text.index("\n}\n", start) + 3
block = text[start:end]
for needle in ("region = _XmRegionCreate();", "if (region == NULL)",
               "if (new_info == NULL)", "DSMEndUpdate(dsm, widget);"):
    if needle not in block:
        raise SystemExit(f"CreateInfo is missing {needle}")

print("drop-site record construction handles allocation failure")
