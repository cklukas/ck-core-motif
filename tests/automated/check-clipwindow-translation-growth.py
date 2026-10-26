#!/usr/bin/env python3
"""Check translation-buffer growth respects Xt's Cardinal-sized allocator."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/ClipWindow.c").read_text(
    encoding="utf-8"
)

assert "required > (size_t)UINT_MAX" in source
assert "next > (size_t)UINT_MAX / 2" in source
assert "new_buffer == NULL" in source
print("Clip-window translation growth is bounded for Xt allocation")
