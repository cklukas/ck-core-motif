#!/usr/bin/env python3
"""Check List translation concatenation uses checked size arithmetic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text()
start = source.index("static void\nClassPartInitialize")
end = source.index("static void\n", start + 24)
region = source[start:end]

assert "size_t first_length = strlen(ListXlations1);" in region
assert "size_t second_length = strlen(ListXlations2);" in region
assert "first_length > SIZE_MAX - second_length - 1" in region
assert "xlat_size = first_length + second_length + 1;" in region
assert "ALLOCATE_LOCAL(xlat_size)" in region
print("List translation concatenation checks its allocation size")
