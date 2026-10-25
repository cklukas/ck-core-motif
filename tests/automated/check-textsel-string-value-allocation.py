#!/usr/bin/env python3
"""Check Text XA_STRING conversion handles returned-value allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextSel.c").read_text()
allocation = "*value = (XtPointer) XtMalloc(tmp_prop.nitems ?"
start = source.index(allocation)
guard = source.index("if (*value == NULL)", start)
cleanup = source.index("if (tmp_prop.value != NULL) XFree((char*)tmp_prop.value);", guard)
assert start < guard < cleanup
assert "*length = 0;" in source[guard:source.index("return False;", guard)]
print("PASS Text XA_STRING value allocation failure is handled")
