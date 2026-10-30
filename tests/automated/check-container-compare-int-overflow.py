#!/usr/bin/env python3
"""Check Container's integer comparator avoids signed subtraction overflow."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("CompareInts(\n", source.index("CompareInts (Private qsort Function)"))
end = source.index("Isqrt(", start)
region = source[start:end]
assert "return (i1 > i2) - (i1 < i2);" in region
assert "i1 - i2" not in region
print("PASS Container integer comparison avoids signed subtraction overflow")
