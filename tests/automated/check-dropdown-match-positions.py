#!/usr/bin/env python3
"""Check DropDown list positioning validates the match-position array."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropDown.c").read_text()
start = source.index("Makes the first selected item")
end = source.index("\n    XtFree((char *) sel_table);", start)
body = source[start:end]

assert "if (num > 0 && pos_list != NULL)" in body
assert "if (num > 0)" not in body

print("DropDown list positioning guards a missing match-position array")
