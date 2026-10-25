#!/usr/bin/env python3
"""Check TabStack initialization handles tear-off label-copy failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TabStack.c").read_text()
start = source.index("static void\n#ifndef _NO_PROTO\nInitialize(")
end = source.index("\n\n/*\n * Function:\n *\tDestroy", start)
region = source[start:end]

assert "#ifdef TEAR_OFF_TABS\n    XmString           replacement;" in region
copy = "replacement = XmStringCopy(XmTabStack_tear_off_label(ts));"
fallback = 'XmStringCreateLocalized("")'
assert copy in region
assert fallback in region
assert region.index(copy) < region.index(fallback)
assert "XmTabStack_tear_off_label(ts) = replacement != NULL" in region
print("TabStack initialization handles tear-off label allocation failure")
