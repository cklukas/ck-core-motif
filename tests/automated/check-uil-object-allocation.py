#!/usr/bin/env python3
"""Check UIL object and section semantic allocations are guarded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarObj.c").read_text()

for function, next_function in (
    ("void\t\tsar_create_object", "void\t\tsar_create_child"),
    ("void\t\tsar_create_child", "void\tsar_link_section"),
):
    start = source.index(function)
    end = source.index(next_function, start)
    body = source[start:end]
    allocation = body.index("sem_allocate_node")
    guard = body.index("if (obj_entry == NULL)", allocation)
    dereference = body.index("obj_entry->", guard)
    assert guard < dereference

start = source.index("void\tsar_link_section")
end = source.index("\f", start)
body = source[start:end]
allocation = body.index("sem_allocate_node")
guard = body.index("if (section_entry == NULL)", allocation)
assert guard < body.index("section_entry->next", guard)
print("UIL object and section allocations are guarded")
