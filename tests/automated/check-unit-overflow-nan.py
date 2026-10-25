#!/usr/bin/env python3
"""Check that unit conversion treats NaN as an overflow."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ResInd.c").read_text(
    encoding="utf-8"
)
start = source.index("#define OVERFLOW(float_value)")
end = source.index("/********", start)
macro = source[start:end]
assert "(float_value) != (float_value)" in macro
assert "INT_MAX" in macro
print("PASS unit conversion rejects NaN overflow")
