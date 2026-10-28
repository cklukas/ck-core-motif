#!/usr/bin/env python3
"""Check the central UIL symbol-node allocator guards XtCalloc failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarObj.c").read_text()
start = source.index("sym_entry_type\t* sem_allocate_node")
end = source.index("\n\f\n/*", start)
body = source[start:end]

assert "node_ptr = (sym_entry_type *) XtCalloc (1, node_size);" in body
assert "if (node_ptr == NULL)" in body
assert 'XtError ("unable to allocate UIL symbol node");' in body
assert "return NULL;" in body
assert body.index("if (node_ptr == NULL)") < body.index("node_ptr->header.w_node_size")
print("UIL symbol-node allocation failure is checked before initialization")
