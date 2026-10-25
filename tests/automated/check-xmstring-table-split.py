#!/usr/bin/env python3
"""Check XmString table splitting guards result-table allocations."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c").read_text()
start = source.index("XmStringToXmStringTable(")
first = source.index("*table = (XmStringTable)XtMalloc(sizeof(XmString));", start)
first_guard = source.index("if (*table == NULL)", first)
second = source.index("*table = (XmStringTable)XtMalloc(count * sizeof(XmString));", first_guard)
second_guard = source.index("if (*table == NULL)", second)
assert first < first_guard < second < second_guard
assert source[first_guard:second_guard].count("_XmProcessUnlock();") >= 1
print("PASS XmString table splitting guards allocations")
