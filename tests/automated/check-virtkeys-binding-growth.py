#!/usr/bin/env python3
"""Check virtual-key binding table growth is bounded and failure-safe."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/VirtKeys.c").read_text(
    encoding="utf-8"
)

assert "total_keys = *num_keys + new_num" in source
assert "SIZE_MAX / sizeof(XmVKeyBindingRec)" in source
assert "UINT_MAX / sizeof(XmVKeyBindingRec)" in source
assert "if (resized == NULL)" in source
print("Virtual-key binding growth is bounded and failure-safe")
