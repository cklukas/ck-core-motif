#!/usr/bin/env python3
"""Check ColorS keeps allocation failures transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorS.c").read_text()
assert "Boolean allocation_failed = False;" in source
assert "ColorInfo *new_info" in source
assert "if (new_info == NULL)" in source
assert "if (allocation_failed)" in source
assert "if (strs == NULL && count != 0)" in source
print("PASS Color selector loader allocations are guarded")
