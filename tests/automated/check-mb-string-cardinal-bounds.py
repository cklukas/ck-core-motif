#!/usr/bin/env python3
"""Check that MB XmString conversion bounds final Xt allocation sizes."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "XmExtUtil.c"
text = source.read_text(encoding="utf-8")
start = text.index("_XmGetMBStringFromXmString")
end = text.index("\n/*\tFunction Name:", start + 1)
region = text[start:end]

required = (
    "length >= (size_t)UINT_MAX",
    "XtMalloc((Cardinal)(length + 1))",
)
missing = [item for item in required if item not in region]
if missing or region.count("length >= (size_t)UINT_MAX") != 2:
    raise SystemExit("MB XmString Cardinal allocation bounds missing")

print("MB XmString conversion bounds Xt allocation size")
