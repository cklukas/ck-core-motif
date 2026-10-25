#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/IconBox.c").read_text()
start = source.index("static void\nGetCellFromXY")
end = source.index("\n/*\tFunction Name: Find Nearest Cell Location", start)
region = source[start:end]
assert "unsigned int cell_width" in region
assert "unsigned int cell_height" in region
assert "if (!cell_width) cell_width = 1;" in region
assert "if (!cell_height) cell_height = 1;" in region
assert "(int)cell_width" in region
assert "(int)cell_height" in region
print("IconBox cell-span guards present")
