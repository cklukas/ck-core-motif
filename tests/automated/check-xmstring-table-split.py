#!/usr/bin/env python3
"""Check XmString table splitting writes through a valid allocated table."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c").read_text()
start = source.index("XmStringToXmStringTable(")
assert "(*table)[0] = XmStringCopy(string);" in source[start:]
assert "*table[0] = XmStringCopy(string);" not in source[start:]
allocation = source.index("*table = (XmStringTable)XtMalloc(sizeof(XmString));", start)
guard = source.index("if (*table == NULL)", allocation)
assert allocation < guard
allocation = source.index("*table = (XmStringTable)XtMalloc(\n\t  (Cardinal)(count * sizeof(XmString)));", start)
guard = source.index("if (*table == NULL)", allocation)
assert allocation < guard
print("PASS XmString table splitting handles allocation and indexing safely")
