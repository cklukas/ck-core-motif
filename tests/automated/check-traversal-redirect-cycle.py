#!/usr/bin/env python3
"""Check traversal redirect cycles retain the current focus widget."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Traversal.c").read_text()
start = source.index("\nRedirectTraversal(") + 1
end = source.index("\n}\n", start) + 3
region = source[start:end]
guard = region.index("if (pass >= 255)")
tail = region[guard:]
assert "return new_focus;" in tail.split("/* Check ancestors", 1)[0]
assert "assert(FALSE)" not in tail.split("/* Check ancestors", 1)[0]
print("PASS traversal redirect-cycle fallback")
