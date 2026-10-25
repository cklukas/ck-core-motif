#!/usr/bin/env python3
"""Check primary-selection state allocation handles failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextSel.c").read_text()
allocation = "XtMalloc((unsigned) sizeof(_XmTextPrimSelect));"
start = source.index(allocation)
guard = source.index("if (prim_select == NULL)", start)
unlock = source.index("_XmProcessUnlock();", guard)
return_stmt = source.index("return;", unlock)
assert start < guard < unlock < return_stmt
assert "XtFree((char *)ds->value);" in source[unlock:return_stmt]
print("PASS primary-selection state allocation failure is handled")
