#!/usr/bin/env python3
"""Verify Scale caps calculated metrics at the legacy Dimension boundary."""

from pathlib import Path


source = Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Scale.c"
text = source.read_text(encoding="utf-8")

assert "ScaleBoundedDimension(int64_t value)" in text
assert "if (value > USHRT_MAX) return (Dimension)USHRT_MAX;" in text

# Height, ascent, descent, and width retain their font measurements in int
# until the one intentional conversion back to Motif's legacy Dimension.
assert text.count("register int tmp_max, tmp_min, result;") == 4
assert text.count("return ScaleBoundedDimension(result);") == 4

# The title/tick padding and scrollbar natural extents use the same boundary.
assert text.count("return ScaleBoundedDimension(MAX(tmp1, tmp2));") == 2
assert text.count("return ScaleBoundedDimension(tmp);") == 2

print("Scale metric dimensions are explicitly bounded")
