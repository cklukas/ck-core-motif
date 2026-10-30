#!/usr/bin/env python3
"""Check TabStack child-index scans start from zero."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TabStack.c").read_text()
start = source.index("TabToIndex(")
start = source.index("{", start)
end = source.index("\n}", start) + 2
body = source[start:end]

assert "int i = 0;" in body
assert "child_index < tab->composite.num_children" in body
assert "if (*kid == child) return i;" in body
print("PASS TabStack child-index scans initialize their index")
