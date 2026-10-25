#!/usr/bin/env python3
"""Check List drag conversion handles target-array allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text()
standard = "Atom *targs = XmeStandardTargets(w, 5, &target_count);"
exported = "Atom *targs = (Atom *) XtMalloc(sizeof(Atom) * 5);"
for allocation in (standard, exported):
    start = source.index(allocation)
    guard = source.index("if (targs == NULL)", start)
    write = source.index("value = (XtPointer) targs;", guard)
    assert start < guard < write
    assert "cs->status = XmCONVERT_REFUSE;" in source[guard:write]
print("PASS List drag target allocation failure is handled")
