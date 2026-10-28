#!/usr/bin/env python3
"""Check XmStringNConcat rejects an overflowing copy limit."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringObso.c").read_text()
start = source.index("XmString \nXmStringNConcat")
end = source.index("XmString \nXmStringNCopy", start)
region = source[start:end]

assert "if (n < 0)" in region
assert "first_length = XmStringLength(first);" in region
assert "n > INT_MAX - first_length" in region
assert "XmStringNCopy(tmp, first_length + n)" in region
print("XmStringNConcat rejects signed copy-limit overflow")
