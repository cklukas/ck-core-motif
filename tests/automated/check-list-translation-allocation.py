#!/usr/bin/env python3
"""Check List class translation assembly handles local allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text()
allocation = "ALLOCATE_LOCAL(xlat_size);"
start = source.index(allocation)
guard = source.index("if (xlats == NULL)", start)
copy = source.index("strcpy(xlats, ListXlations1);", guard)
assert start < guard < copy
assert "first_length > SIZE_MAX - second_length - 1" in source
assert "xlat_size = first_length + second_length + 1;" in source
assert "return;" in source[guard:copy]
print("PASS List translation allocation failure is handled")
