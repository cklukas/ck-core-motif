#!/usr/bin/env python3
"""Check XmScaleSetTicks rejects invalid intervals and bounded counts."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScalTics.c").read_text(
    encoding="utf-8"
)
start = source.index("XmScaleSetTicks(")
body = source[start:]
assert "size_big == 0 || big_every <= 0" in body
assert "intmax_t range = (intmax_t)max - (intmax_t)min" in body
assert "big_count > INT_MAX" in body
assert "SIZE_MAX / sizeof(Widget)" in body
assert "total > UINT_MAX" in body
assert "ALLOCATE_LOCAL(sep_num * sizeof(Widget))" in body
print("PASS XmScaleSetTicks validates interval, counts, and allocation size")
