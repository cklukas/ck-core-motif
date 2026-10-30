#!/usr/bin/env python3
"""Check TabStack tear-off menu selection validates widget data."""

from pathlib import Path


source = Path("lib/Xm/TabStack.c").read_text(encoding="utf-8")
start = source.rindex("MenuSelectCallback(widget")
end = source.index("\n}", start)
body = source[start:end]
assert "if (widget == NULL)" in body
assert "if (data == NULL)" in body
assert "XmTabStackSelectTab((Widget)data)" in body

print("TabStack menu selection guard present")
