#!/usr/bin/env python3
"""Check PictureVerifyCallback handles its splice allocation failure."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DataF.c").read_text(
    encoding="utf-8"
)

required = (
    "newptr = XtMalloc(curr_length + insert_length + 2);",
    "if (newptr == NULL)",
    "cbs->doit = False;",
    "XtFree(curr);",
)
missing = [item for item in required if item not in source]
if missing:
    raise SystemExit("picture splice allocation guard missing: " + ", ".join(missing))

print("DataField picture-allocation checks passed")
