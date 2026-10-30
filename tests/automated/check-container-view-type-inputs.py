#!/usr/bin/env python3
"""Check Container view/icon/detail helpers validate widget and node state."""
from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Container.c").read_text(
    encoding="utf-8"
)
for name in ("SetViewType(\n", "GetIconWidth(\n"):
    start = source.rindex(name)
    region = source[start : source.index("\n}\n", start)]
    assert "if (cwid == NULL)" in region
detail_start = source.rindex("GetDefaultDetailCount(\n")
detail_region = source[detail_start : source.index("\n}\n", detail_start)]
assert "if (wid == NULL)" in detail_region
assert "if (cwid == NULL)" in detail_region

print("PASS Container view, icon, and detail helpers validate inputs")
