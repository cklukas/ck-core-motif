#!/usr/bin/env python3
"""Check traversal exclusive-tab-list growth and shrink failure handling."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TraversalI.c").read_text()
add = source[source.index("_XmTabListAdd"):source.index("_XmTabListDelete")]
delete = source[source.index("_XmTabListDelete"):source.index("\nstatic int\nSearchTabList", source.index("_XmTabListDelete"))]
assert "Widget *new_list" in add
assert "if (new_list == NULL)" in add
assert "graph->excl_tab_list = new_list" in add
assert "graph->tab_list_alloc = new_alloc" in add
assert "if (new_list != NULL)" in delete
assert "graph->num_tab_list <" in delete
print("PASS traversal tab-list growth handles allocation failure")
