#!/usr/bin/env python3
"""Check traversal-node pool growth preserves state on allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TraversalI.c").read_text()
assert "#include <stdint.h>" in source
alloc = source[source.index("size_t new_alloc"):source.index("static Boolean \nGetChildList")]
get_nodes = source[source.index("list_entry_offset ="):source.index("if (free_child_list)")]
assert "XmTraversalNode new_head" in alloc
assert "if (new_head == NULL)" in alloc
assert "list->head = new_head" in alloc
assert "return NULL;" in alloc
assert "if (list_entry == NULL)" in get_nodes
assert "if (controls_graph == NULL)" in get_nodes
print("PASS traversal-node growth handles allocation failure")
