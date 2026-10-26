#!/usr/bin/env python3
"""Check that XmString segment-copy allocation is bounded and checked."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/XmString.c").read_text(
    encoding="utf-8"
)

assert "a_lc <= 0" in source
assert "a_lc > SIZE_MAX / sizeof(*segs)" in source
assert "a_lc > UINT_MAX / sizeof(*segs)" in source
assert "if (segs == NULL)" in source
assert "original_segs" in source
print("XmString segment-copy allocation is bounded")
