#!/usr/bin/env python3
"""Check that tear-off cancel-key allocation is bounded and checked."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/TearOff.c").read_text(
    encoding="utf-8"
)

assert "num_keys < 0" in source
assert "num_keys > SIZE_MAX / sizeof(KeyCode)" in source
assert "num_keys > UINT_MAX / sizeof(KeyCode)" in source
assert "KCancel == NULL && num_keys != 0" in source
print("TearOff cancel-key allocation is bounded")
