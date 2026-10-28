#!/usr/bin/env python3
"""Check XmPrintToFile validates output paths and cleans fork failures."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "PrintS.c").read_text()
start = source.index("XmPrintToFile(")
end = source.index("\n}\n", start) + 3
region = source[start:end]
assert "pdpy == NULL || file_name == NULL || *file_name == '\\0'" in region
assert "remove(file_name)" in region
assert region.index("pdpy == NULL") < region.index("fopen(file_name")
print("PrintShell validates output paths and cleans fork failures")
