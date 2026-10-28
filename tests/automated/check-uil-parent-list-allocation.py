#!/usr/bin/env python3
"""Check UIL parent-list traversal handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarObj.c").read_text()
start = source.index("void parent_list_traverse")
end = source.index("void\t\tsar_update_parent_list", start) if "void\t\tsar_update_parent_list" in source[start:] else source.index("\f", start)
body = source[start:end]

assert body.count("if (parent_node == NULL)") == 2
assert body.count("continue;") >= 2
assert body.count("parent_node->next") == 2
for guard in ("if (parent_node == NULL)"):
    assert body.index(guard) < body.index("parent_node->next")
print("UIL parent-list allocations are guarded")
