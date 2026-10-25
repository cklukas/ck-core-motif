#!/usr/bin/env python3
"""Check Text TARGETS conversion handles standard-target allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextSel.c").read_text()
allocation = "Atom *targs = XmeStandardTargets(w, 10, &target_count);"
start = source.index(allocation)
guard = source.index("if (targs == NULL)", start)
first_write = source.index("targs[target_count]", guard)
assert start < guard < first_write
assert "*value = NULL;" in source[guard:first_write]
assert "return False;" in source[guard:first_write]
print("PASS Text TARGETS allocation failure is handled")
