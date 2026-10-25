#!/usr/bin/env python3
"""Check XmString table parsing handles table allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c").read_text()
allocation = "strs = (XmStringTable)XtMalloc(\n      (Cardinal)((size_t)count * sizeof(XmString)));"
start = source.index(allocation)
guard = source.index("if (strs == NULL)", start)
write = source.index("strs[i] = XmStringParseText", guard)
assert start < guard < write
assert "return(NULL);" in source[guard:write]
print("PASS XmString table parse allocation failure is handled")
