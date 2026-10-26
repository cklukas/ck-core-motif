#!/usr/bin/env python3
"""Check modify-verify conversion rejects a failed text-block allocation."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DataF.c").read_text(
    encoding="utf-8"
)

required = (
    "newblock.ptr = (char *) XtMalloc((unsigned) newblock.length +",
    "if (newblock.ptr == NULL)",
    "return False;",
)
missing = [item for item in required if item not in source]
if missing:
    raise SystemExit("modify-verify allocation guard missing: " + ", ".join(missing))

print("DataField modify-verify allocation checks passed")
