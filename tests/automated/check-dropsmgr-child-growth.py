#!/usr/bin/env python3
"""Check drop-site child-list growth is bounded and failure-safe."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/DropSMgrI.c").read_text(
    encoding="utf-8"
)

assert "num_children > USHRT_MAX - CHILDREN_INCREMENT" in source
assert "SIZE_MAX / sizeof(XmDSInfo)" in source
assert "new_children == NULL" in source
print("Drop-site child-list growth is bounded and failure-safe")
