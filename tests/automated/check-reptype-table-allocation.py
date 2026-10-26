#!/usr/bin/env python3
"""Check dynamic representation-type table sizing for Xt allocation."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/RepType.c").read_text(
    encoding="utf-8"
)

assert "SIZE_MAX / sizeof(XmRepTypeEntryRec)" in source
assert "UINT_MAX / sizeof(XmRepTypeEntryRec)" in source
assert "new_dynamic_types == NULL" in source
print("Representation-type table allocation is bounded")
