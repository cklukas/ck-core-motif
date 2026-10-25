#!/usr/bin/env python3
"""Check List class translation assembly handles local allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text()
allocation = "ALLOCATE_LOCAL(strlen(ListXlations1) + strlen(ListXlations2) + 1);"
start = source.index(allocation)
guard = source.index("if (xlats == NULL)", start)
copy = source.index("strcpy(xlats, ListXlations1);", guard)
assert start < guard < copy
assert "return;" in source[guard:copy]
print("PASS List translation allocation failure is handled")
