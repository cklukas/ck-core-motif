#!/usr/bin/env python3
"""Check checked allocation for Mwm workspace property names."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmProperty.c").read_text(
    encoding="utf-8"
)
assert "size_t prefixLength;" in source
assert "size_t nameLength;" in source
assert "prefixLength + nameLength > SIZE_MAX - 2" in source
assert "prefixLength + nameLength + 2 > UINT_MAX" in source
assert "XtMalloc ((Cardinal) totalLength)" in source
assert source.count('Warning("Workspace property name is too long")') == 2
assert "int len;" not in source
print("Mwm workspace property names use checked size arithmetic")
