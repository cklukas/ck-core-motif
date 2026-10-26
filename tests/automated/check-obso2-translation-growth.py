#!/usr/bin/env python3
"""Check legacy translation-buffer growth stays within Xt's size range."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/Obso2_0.c").read_text(
    encoding="utf-8"
)

assert "new_capacity > (size_t)UINT_MAX / 2" in source
assert "new_capacity < required" in source
assert "new_buffer == NULL" in source
print("Legacy translation growth is bounded and exact-size fallback is retained")
