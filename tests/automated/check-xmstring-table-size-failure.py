#!/usr/bin/env python3
"""Check that XmString-table conversion frees results on size failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c").read_text(
    encoding="utf-8"
)
start = source.index("CvtStringToXmStringTable(", source.index("CvtStringToXmStringTable(" ) + 1)
end = source.index("/****************", start)
region = source[start:end]
branch = region.index("if (to_val->size < sizeof(XtPointer))")
failure = region[branch:region.index("return False;", branch) + len("return False;")]
assert "XmStringFree(table[cleanup])" in failure
assert "XtFree((char *)table)" in failure
print("PASS XmString-table size failure frees allocated results")
