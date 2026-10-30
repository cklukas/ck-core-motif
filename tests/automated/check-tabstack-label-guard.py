#!/usr/bin/env python3
"""Check TabStack's synthetic label getter validates callback inputs."""

from pathlib import Path


source = Path("lib/Xm/TabStack.c").read_text(encoding="utf-8")
start = source.rindex("static void Get_tabLabelString")
end = source.index("\n}", start)
body = source[start:end]
assert "widget == NULL || value == NULL" in body
assert "(*value) = (XtArgVal) NULL" in body

print("TabStack label getter guard present")
