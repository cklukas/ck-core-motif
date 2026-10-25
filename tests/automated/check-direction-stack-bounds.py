#!/usr/bin/env python3
"""Check compound-text direction-stack growth respects Xt width."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "ResEncod.c").read_text()

assert "(size_t)new_size > SIZE_MAX / sizeof(*new_stack)" in source
assert "(size_t)new_size > UINT_MAX / sizeof(*new_stack)" in source
assert "(Cardinal)((size_t)new_size * sizeof(*new_stack))" in source

print("compound-text direction-stack growth is bounded")
