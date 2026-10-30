#!/usr/bin/env python3
"""Check Container recursive tree helpers validate child and parent state."""
from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
reset_start = source.rindex("ContainerResetDepths\n")
reset_region = source[reset_start : source.index("\n}\n", reset_start)]
assert "if (child == NULL)" in reset_region
assert "if (cc == NULL)" in reset_region

desc_start = source.rindex("ContainerIsDescendant")
desc_region = source[desc_start : source.index("\n}\n", desc_start)]
assert "if (containerChild == NULL || newEntryParent == NULL)" in desc_region
assert "if (nepc == NULL || nepc->node_ptr == NULL)" in desc_region

print("PASS Container recursive tree helpers validate child and parent state")
