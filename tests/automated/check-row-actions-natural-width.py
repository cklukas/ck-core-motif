#!/usr/bin/env python3
"""Check RowActions natural width saturates before unsigned wraparound."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RowActions.c").read_text()
start = source.index("static void natural(")
end = source.index("\n}\n", start) + 2
body = source[start:end]

assert "total > (unsigned long)USHRT_MAX - child_width" in body
assert "total = USHRT_MAX;" in body
assert "total += child_width;" in body
print("PASS RowActions natural width saturates before overflow")
