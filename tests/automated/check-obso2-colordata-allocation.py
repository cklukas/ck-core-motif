#!/usr/bin/env python3
"""Check legacy color-data table sizing for Xt allocation."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/Obso2_0.c").read_text(
    encoding="utf-8"
)

assert source.count("UINT_MAX / sizeof(XmColorData *)") >= 1
assert "new_default_set == NULL" in source
print("Legacy color-data table allocation is bounded")
