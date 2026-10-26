#!/usr/bin/env python3
"""Check Hierarchy child-list growth honors Xt's Cardinal limit."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Hierarchy.c").read_text()
start = source.index("list_bytes = list_count * sizeof(*new_list)")
body = source[start:start + 260]
assert "list_bytes > (size_t)UINT_MAX" in body
assert "XtRealloc" in body
print("PASS Hierarchy child-list allocation bounds")
