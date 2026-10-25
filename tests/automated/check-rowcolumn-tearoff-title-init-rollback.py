#!/usr/bin/env python3
"""Check RowColumn initialization handles tear-off title-copy failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RowColumn.c").read_text()
start = source.index("static void \nInitialize(")
end = source.index("\n}\n\n/* ARGSUSED */", start)
region = source[start:end]

copy = "replacement = XmStringCopy(RC_TearOffTitle(m));"
fallback = 'XmStringCreateLocalized("")'
assert "XmString replacement;" in region
assert copy in region
assert fallback in region
assert region.index(copy) < region.index(fallback)
assert "RC_TearOffTitle(m) = replacement != NULL" in region
print("RowColumn initialization handles tear-off title allocation failure")
