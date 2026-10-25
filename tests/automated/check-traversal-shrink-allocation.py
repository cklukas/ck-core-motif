#!/usr/bin/env python3
"""Check traversal graph pool shrinking preserves storage on failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TraversalI.c").read_text()
body = source[source.index("GetNodeList(top_wid"):source.index("LinkNodeList(trav_list)")]
assert "trav_list->num_alloc > XmTRAV_LIST_ALLOC_INCREMENT" in body
assert "size_t new_alloc" in body
assert "XmTraversalNode new_head" in body
assert "if (new_head != NULL)" in body
assert "trav_list->head = new_head" in body
assert "trav_list->num_alloc = new_alloc" in body
print("PASS traversal graph shrinking is transactional")
