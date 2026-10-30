#!/usr/bin/env python3
"""Ensure XPM buffer serialization checks space for its final terminator."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrBufFrI.c").read_text(
    encoding="utf-8"
)
guard = 'if (ptr_size < used_size || ptr_size - used_size < sizeof("};\\n"))'
assert guard in source
guard_pos = source.index(guard)
copy_pos = source.index('strcpy(ptr + used_size, "};\\n");')
assert guard_pos < copy_pos
print("XPM buffer serialization checks final terminator capacity")
