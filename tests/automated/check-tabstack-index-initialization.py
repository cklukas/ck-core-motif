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
unused_start = source.index("IndexToTab(tab, idx)")
unused_end = source.index("\n#endif", source.index("return( (Widget)NULL );", unused_start))
unused = source[unused_start:unused_end]
assert "WidgetList kid;" in unused
assert "int i = 0;" in unused
assert "kid = tab->composite.children;" in unused
assert unused.index("if( tab == NULL") < unused.index("kid = tab->composite.children;")
print("PASS TabStack child-index scans initialize their index")
