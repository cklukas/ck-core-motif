#!/usr/bin/env python3
"""Check edit-resources child enumeration handles count and allocation limits."""

from pathlib import Path


source = Path("lib/Xm/EditresCom.c").read_text()

assert "parent->core.num_popups > INT_MAX - num_children" in source
assert "cw->composite.num_children > INT_MAX - num_children" in source
assert "SIZE_MAX / sizeof(Widget)" in source
assert "UINT_MAX / sizeof(Widget)" in source
assert "if (*children == NULL)" in source

print("edit-resources child enumeration bounds are guarded")
