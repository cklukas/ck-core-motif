#!/usr/bin/env python3
"""Check that scrolled-list name construction handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(
    encoding="utf-8"
)
start = source.index("XmCreateScrolledList(")
end = source.index("/* Selection API", start)
region = source[start:end]
assert "XmStrlen(name) > SIZE_MAX - 3" in region
assert "s = (char*) ALLOCATE_LOCAL(name_size)" in region
assert "if (s == NULL)" in region
print("PASS scrolled-list name allocation is guarded")
