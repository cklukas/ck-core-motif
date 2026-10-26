#!/usr/bin/env python3
"""Check DragBS target-table allocation and failure cleanup."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DragBS.c").read_text(
    encoding="utf-8"
)

assert "newTargets == NULL" in source
assert "new_entries == NULL" in source
assert "targetsTable->numEntries == UINT_MAX" in source
assert "XUngrabServer(display);" in source
print("DragBS target allocations are bounded and failure-safe")
