#!/usr/bin/env python3
"""Check that optimized XmString concatenation avoids length wraparound."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("XmString \nXmStringConcatAndFree(")
end = source.index("\n/*\n * _XmStringContext", start)
region = source[start:end]

assert "a_len < (1U << BYTE_COUNT_BITS)" in region
assert "b_len < (1U << BYTE_COUNT_BITS) - a_len" in region
assert "(a_len + b_len) < (1 << BYTE_COUNT_BITS)" not in region
print("PASS XmString optimized concatenation length bound avoids wraparound")
