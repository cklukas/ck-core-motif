#!/usr/bin/env python3
"""Check drop-down selection conversion guards its table allocation."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "DropDown.c"
text = source.read_text(encoding="utf-8")
start = text.index("sel_table = (XmStringTable) XtMalloc")
end = text.index("\n\t}\n\telse", start)
region = text[start:end]
assert "if (sel_table == NULL)" in region
assert "error = TRUE;" in region
assert "count = 0;" in region
print("drop-down selection-table allocation is guarded")
