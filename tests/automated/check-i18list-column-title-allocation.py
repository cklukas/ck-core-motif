#!/usr/bin/env python3
"""Check I18List column-title copies fail transactionally."""
from pathlib import Path

source = Path("lib/Xm/I18List.c").read_text()
start = source.index("static Boolean\nCopyColumnTitles(")
end = source.index("\n/*\n * Function:\n *\tFreeColumnTitles", start)
body = source[start:end]
assert "SIZE_MAX / sizeof(*copy)" in body
assert "UINT_MAX / sizeof(*copy)" in body
assert "if (copy == NULL)" in body
assert "if (copy[i] == NULL)" in body
assert "XmI18List_column_titles(ilist) = (XmStringTable) NULL;" in body
assert "return False;" in body
print("I18List column-title copies are transactional")
