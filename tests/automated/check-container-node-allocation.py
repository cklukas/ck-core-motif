#!/usr/bin/env python3
"""Check that Container child-node allocation failure is handled safely."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()

start = source.index("static void\nConstraintInitialize(")
end = source.index("\n/************************************************************************\n * ConstraintDestroy", start)
region = source[start:end]

assert "node = NewNode(ncwid);" in region
assert "if (node == NULL)" in region
assert region.index("if (node == NULL)") < region.index("InsertNode(node)")

new_node = source.index("static  CwidNode\nNewNode(")
new_node_end = source.index("\n/************************************************************************\n * InsertNode", new_node)
new_node_region = source[new_node:new_node_end]
assert "new_node = (CwidNode)XtCalloc" in new_node_region
assert "if (new_node == NULL)" in new_node_region
assert new_node_region.index("if (new_node == NULL)") < new_node_region.index("new_node->widget_ptr")

insert = source.index("InsertNode(\n\tCwidNode\tnode)")
insert_end = source.index("\n/************************************************************************\n * SeverNode", insert)
insert_region = source[insert:insert_end]
assert "if (node == NULL)" in insert_region
assert "if (parent_node == NULL)" in insert_region
assert "c->entry_parent = NULL;" in insert_region

print("PASS Container child-node allocation failure is handled")
