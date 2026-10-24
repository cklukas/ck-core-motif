#!/usr/bin/env python3
"""Check guarded growth of the XmString table resource converter."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c"
text = source.read_text(encoding="utf-8")
start = text.index("CvtStringToXmStringTable(", 10000)
end = text.index("\n/*ARGSUSED*/\nstatic void", start)
region = text[start:end]

required = (
    "UINT_MAX / (2 * sizeof(XmString))",
    "UINT_MAX / sizeof(XmString)",
    "for (cleanup = 0; cleanup < str_no; ++cleanup)",
    "table_size = str_no + 1",
)
missing = [item for item in required if item not in region]
if missing:
    raise SystemExit("XmString table capacity checks missing: " + ", ".join(missing))

print("XmString table growth is bounded")
